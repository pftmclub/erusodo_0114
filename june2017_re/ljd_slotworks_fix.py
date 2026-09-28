"""The ljd slotworks fix (Phase 4 of RANOX_LUA_DECOMPILE_FIX_PLAN.md).

Not a module. This file is slotworks.py code: ljd_fixed.install_fix() execs it
inside ljd.ast.slotworks's own namespace (so every name below resolves the way
it would in slotworks.py), and `ljd_fixed.py --write-patch` splices the same
text into slotworks.py to make ljd_slotworks.patch. One text for both is what
makes the hook and the patch the same change. Everything after the guard
below is copied into the patch verbatim.

The defect (RANOX_LUA_DECOMPILE_BUG.md, "Root cause"): slotworks inlines a
temporary when the collection it just ran counts exactly one read of it, but
every collection sees only part of the value's live range. A read it cannot
see, or skips, is not counted, so a value read once inside the view and again
outside it is folded into the one read that was counted, and the other read is
left naming a register nothing writes. Phase 3 found four such views (the
id == -1 skip in _fill_simple_refs, the per-block recheck, _cleanup_ast over
structured code, and collections rooted at a sub-tree); closing one hands the
value to the next.

The fix closes all four where the write is actually deleted, instead of
patching each view. Before _eliminate_simple_cases deletes `slotN = <expr>` it
asks one question. Is there a read of slotN that this definition reaches, with
no write of slotN in between, other than the reads the collection consumed?
- It answers over the whole enclosing function as it stands at that moment:
  warped blocks, structured statements, or the mix the unwarper leaves between
  its steps. It does not answer over the collection's view.
- If such a read exists, the assignment is kept.
- If the check can't place the assignment or follow the flow, it also keeps it.
- It also keeps it when a table-constructor ref won't fold, since that ref goes
  on reading the slot.
- For `slotA, slotB = nil` (KNIL), which deletes every destination at once, it
  asks the same question of each of them.
- A closure capturing the register counts as a read, and ISTC/ISFC's copy
  counts as a write on its jump edge.

The same question guards _eliminate_mass_assignments. That is a fifth site,
found in review: _remove_invalid_references drops trailing id == -1 reads the
same way. _eliminate_iterators is left alone. Its slots are the for-in
generator registers, which compiled Lua never reads outside the loop header.

ljd keeps its own decision in two places, where keeping the assignment would
not keep the value:
- unwarper.py:570 collects over a temporary block made of expression-body
  blocks it deletes straight afterwards. Those cases are counted as `exempt`.
- A part of a value-producing and/or, whose value ljd carries in its block's
  test (see _ljdfix_andor_value). Those cases are counted as `andor`.

Set LJD_FIX_LOG=<file> for one line per veto / exemption.
"""
if __name__ != 'ljd.ast.slotworks':
    raise ImportError('ljd_slotworks_fix.py is slotworks.py code, not a module - see ljd_fixed.py')

# ==== ljd_fixed: slotworks fix. Everything from here on is spliced into slotworks.py by the patch. ====

import sys

_LJDFIX_STATS = {'checked': 0, 'vetoed': 0, 'exempt': 0, 'andor': 0, 'unlocated': 0, 'stale_target': 0, 'unknown_node': 0}
_LJDFIX_FLOWS = {}  # FunctionDefinition -> _LjdfixFlow, valid for one _eliminate_simple_cases batch
_LJDFIX_LOG = os.environ.get('LJD_FIX_LOG')

# Child nodes of each node type, in _accept's order. Warp targets are not
# children (they are edges); FunctionDefinition bodies are skipped by the
# callers that must not look into another function's registers.
_LJDFIX_CHILDREN = {
    nodes.FunctionDefinition: lambda n: (n.arguments, n.statements),
    nodes.TableConstructor: lambda n: (n.array, n.records),
    nodes.ArrayRecord: lambda n: (n.value,),
    nodes.TableRecord: lambda n: (n.key, n.value),
    nodes.Assignment: lambda n: (n.expressions, n.destinations),
    nodes.BinaryOperator: lambda n: (n.left, n.right),
    nodes.UnaryOperator: lambda n: (n.operand,),
    nodes.StatementsList: lambda n: n.contents,
    nodes.IdentifiersList: lambda n: n.contents,
    nodes.RecordsList: lambda n: n.contents,
    nodes.VariablesList: lambda n: n.contents,
    nodes.ExpressionsList: lambda n: n.contents,
    nodes.TableElement: lambda n: (n.key, n.table),
    nodes.FunctionCall: lambda n: (n.arguments, n.function),
    nodes.If: lambda n: [n.expression, n.then_block] + list(n.elseifs) + [n.else_block],
    nodes.ElseIf: lambda n: (n.expression, n.then_block),
    nodes.Block: lambda n: list(n.contents) + [n.warp],
    nodes.ConditionalWarp: lambda n: (n.condition,),
    nodes.IteratorWarp: lambda n: (n.variables, n.controls),
    nodes.NumericLoopWarp: lambda n: (n.index, n.controls),
    nodes.Return: lambda n: (n.returns,),
    nodes.While: lambda n: (n.expression, n.statements),
    nodes.RepeatUntil: lambda n: (n.statements, n.expression),
    nodes.NumericFor: lambda n: (n.variable, n.expressions, n.statements),
    nodes.IteratorFor: lambda n: (n.expressions, n.identifiers, n.statements),
}
_LJDFIX_LEAVES = (nodes.Identifier, nodes.Constant, nodes.Primitive, nodes.MULTRES, nodes.Vararg,
                  nodes.NoOp, nodes.Break, nodes.EndWarp, nodes.UnconditionalWarp)


def _ljdfix_children(node):
    get = _LJDFIX_CHILDREN.get(type(node))
    if get is not None:
        return [c for c in get(node) if c is not None]
    if not isinstance(node, _LJDFIX_LEAVES):
        _LJDFIX_STATS['unknown_node'] += 1
    return ()


def _ljdfix_stamp(ast):
    # Record every node's enclosing FunctionDefinition, once, on the whole file (the
    # main.py:380 pass). Later collections are rooted at sub-trees and don't know it.
    seen = set()
    stack = [(ast, None)]
    while stack:
        node, fn = stack.pop()
        if id(node) in seen:
            continue
        seen.add(id(node))
        node._ljdfix_fn = fn
        if isinstance(node, nodes.ConditionalWarp):
            _ljdfix_stamp_test(node)
        inner = node if isinstance(node, nodes.FunctionDefinition) else fn
        for child in _ljdfix_children(node):
            stack.append((child, inner))


def _ljdfix_stamp_test(warp):
    # builder.py sets _slot on a unary test warp: CD for IST/ISF (it tests that
    # register), A for ISTC/ISFC (it tests D and, on the jump, copies it into A). Tell the
    # two apart now, while the condition is still the bare register: _ljdfix_test is a
    # test of a register's own value, _ljdfix_copy a write of A on the jump edge.
    slot = getattr(warp, '_slot', None)
    cond = warp.condition
    if isinstance(cond, nodes.UnaryOperator) and cond.type == nodes.UnaryOperator.T_NOT:
        cond = cond.operand
    # A named local (-g) keeps its register number in .slot, so it counts too.
    if slot is None or not isinstance(cond, nodes.Identifier) or cond.slot is None or cond.slot < 0:
        return
    if cond.slot == slot:
        warp._ljdfix_test = slot
    else:
        warp._ljdfix_copy = slot


def _ljdfix_function_of(root):
    # The function whose code a collection root is. A Block the unwarper made later
    # (unstamped) holds statements that were stamped; leaves are skipped, since only
    # statements and structure are known to belong to one function.
    if root is None or isinstance(root, nodes.FunctionDefinition):
        return root
    fn = getattr(root, '_ljdfix_fn', None)
    stack = list(_ljdfix_children(root))
    while fn is None and stack:
        sub = stack.pop()
        if not isinstance(sub, _LJDFIX_LEAVES):
            fn = getattr(sub, '_ljdfix_fn', None)
        if not isinstance(sub, nodes.FunctionDefinition):
            stack.extend(_ljdfix_children(sub))
    return fn


_LJDFIX_UNKNOWN = object()  # yielded for a node type _LJDFIX_CHILDREN doesn't know
_LJDFIX_CLOSURE = object()  # yielded for a closure that captures the register
_LJDFIX_UV_LOCAL = 0x8000  # LuaJIT PROTO_UV_LOCAL: the upvalue is a local register of the parent


def _ljdfix_reads(roots, slot, flow, here):
    # Every T_SLOT read of `slot` under roots that happens at the point for statement
    # `here`, not looking into nested functions. An expression an elimination moved
    # (_ljdfix_origins, oldest first) is read where it was first computed, as long as
    # that assignment is still in the flow: ljd inlines through chains whose middle
    # states read a register after the next write to it (obj:Make():Go() inlines
    # `slot2 = slot1` past `slot1 = slot1.Go`), and the value meant is the one at the
    # origin. Identifiers no longer in the tree (a method call's deleted self argument,
    # still held by the assignment it was moved out of) read nothing.
    stack = list(roots)
    while stack:
        node = stack.pop()
        if node is None:
            continue
        if isinstance(node, nodes.FunctionDefinition):
            # A closure is created here and captures parent registers by reference.
            if any(uv & _LJDFIX_UV_LOCAL and uv & 0xff == slot for uv in node._upvalues or ()):
                yield _LJDFIX_CLOSURE
            continue
        origins = getattr(node, '_ljdfix_origins', None)
        if origins:
            origin = next((o for o in origins if o in flow.points), None)
            if origin is not None and origin is not here:
                continue
        if isinstance(node, nodes.Identifier):
            if node.type == nodes.Identifier.T_SLOT and node.slot == slot and node in flow.present:
                yield node
            continue
        if type(node) not in _LJDFIX_CHILDREN and not isinstance(node, _LJDFIX_LEAVES):
            yield _LJDFIX_UNKNOWN
            continue
        stack.extend(_ljdfix_children(node))


_LJDFIX_REGISTER_TYPES = (nodes.Identifier.T_SLOT, nodes.Identifier.T_LOCAL)


def _ljdfix_writes(identifiers, slot):
    # A write of the register, whatever ljd calls it: a named local (-g) that reuses
    # the register ends the temporary's range too. Not an upvalue or builtin, whose
    # .slot is something else.
    return any(isinstance(i, nodes.Identifier) and i.type in _LJDFIX_REGISTER_TYPES and i.slot == slot
               for i in identifiers)


class _LjdfixPoint:
    # One program point of a function. kind:
    #   stmt  a simple statement: reads its expressions (and destination sub-expressions),
    #         then writes its slot destinations. An _invalidated (eliminated) one still
    #         does both: it stands for the reads of the expression moved out of it, and
    #         every read after it is one of its own (it passed this check, and its
    #         table-constructor refs stay in place until they are folded in), so it
    #         still ends an earlier definition's range.
    #   read  reads getattr(what, a) for a in attrs (looked up when visited, since an
    #         elimination can replace a warp's condition or a loop's controls)
    #   kill  writes the identifiers in what
    #   iter  an IteratorWarp: reads its controls, then writes its variables
    #   nop   nothing
    # succ: [(point or None, slots written on that edge only)]; None is the function exit.
    __slots__ = ('kind', 'what', 'attrs', 'succ')

    def __init__(self, kind, what=None, attrs=()):
        self.kind = kind
        self.what = what
        self.attrs = attrs
        self.succ = []

    def effect(self, slot):
        # -> (roots whose reads happen here, whether this point then writes slot)
        kind = self.kind
        if kind == 'stmt':
            node = self.what
            if isinstance(node, nodes.Assignment):
                dsts = node.destinations.contents
                roots = list(node.expressions.contents)
                roots += [d for d in dsts if not isinstance(d, nodes.Identifier)]
                return roots, _ljdfix_writes(dsts, slot)
            return (node,), False
        if kind == 'read':
            return [getattr(self.what, a) for a in self.attrs], False
        if kind == 'kill':
            return (), _ljdfix_writes(self.what, slot)
        if kind == 'iter':
            return (self.what.controls,), _ljdfix_writes(self.what.variables.contents, slot)
        return (), False


class _LjdfixFlow:
    # The control flow of one function as the tree stands now. Statement lists hold
    # either Blocks, whose warps are the edges (the warped AST, and whatever the
    # unwarper has not structured yet), or statements, where If / loops / Break /
    # Return give the edges.
    def __init__(self, fn):
        self.points = {}  # simple statement -> its point
        self.blocks = {}  # Block -> (entry point, warp point)
        self.numloops = {}  # NumericLoopWarp.body -> the warp (FORL), for FORI's zero-trip edge
        self.present = set()  # T_SLOT identifiers in the tree, outside eliminated assignments
        stack = [fn.statements]
        while stack:
            node = stack.pop()
            if isinstance(node, nodes.Identifier):
                if node.type == nodes.Identifier.T_SLOT:
                    self.present.add(node)
                continue
            if isinstance(node, nodes.FunctionDefinition) or _is_invalidated(node):
                continue
            if isinstance(node, nodes.Block):
                self.blocks[node] = (_LjdfixPoint('nop'), _LjdfixPoint('nop'))
                if isinstance(node.warp, nodes.NumericLoopWarp):
                    self.numloops[node.warp.body] = node.warp
            stack.extend(_ljdfix_children(node))
        self.entry = self._list(fn.statements.contents, None, None)

    def _list(self, items, exit, brk):
        nxt = exit
        for item in reversed(items):
            nxt = self._item(item, nxt, exit, brk)
        return nxt

    def _edge(self, block, list_exit):
        if block is None:
            return list_exit, ()
        points = self.blocks.get(block)
        if points is None:
            # A target in no statements list. Probably dead, but the flow can't follow it,
            # so a check that reaches it gives up and keeps the assignment.
            _LJDFIX_STATS['stale_target'] += 1
            return _LjdfixPoint('stale', block), ()
        return points[0], ()

    def _item(self, item, nxt, list_exit, brk):
        if isinstance(item, nodes.Block):
            entry, wp = self.blocks[item]
            entry.succ = [(self._list(item.contents, wp, brk), ())]
            warp = item.warp
            if isinstance(warp, nodes.ConditionalWarp):
                wp.kind, wp.what, wp.attrs = 'read', warp, ('condition',)
                jump, _ = self._edge(warp.false_target, list_exit)
                copy = getattr(warp, '_ljdfix_copy', None)
                # builder.py: the jump destination is false_target; ISTC/ISFC copy only on it
                wp.succ = [self._edge(warp.true_target, list_exit), (jump, () if copy is None else (copy,))]
            elif isinstance(warp, nodes.UnconditionalWarp):
                wp.succ = [self._edge(warp.target, list_exit)]
                loop = self.numloops.get(warp.target) if item.loop else None
                if loop is not None and warp.type == nodes.UnconditionalWarp.T_FLOW:
                    # FORI: reads the controls, then enters the body or skips the loop
                    wp.kind, wp.what, wp.attrs = 'read', loop, ('controls',)
                    wp.succ.append(self._edge(loop.way_out, list_exit))
            elif isinstance(warp, nodes.NumericLoopWarp):
                # FORL: the visible index is written only on the way back into the body
                wp.kind, wp.what, wp.attrs = 'read', warp, ('controls',)
                body, _ = self._edge(warp.body, list_exit)
                index = warp.index
                kills = (index.slot,) if isinstance(index, nodes.Identifier) and index.type in _LJDFIX_REGISTER_TYPES else ()
                wp.succ = [(body, kills), self._edge(warp.way_out, list_exit)]
            elif isinstance(warp, nodes.IteratorWarp):
                wp.kind, wp.what = 'iter', warp
                wp.succ = [self._edge(warp.body, list_exit), self._edge(warp.way_out, list_exit)]
            else:
                wp.succ = [(list_exit, ())]
            return entry

        if isinstance(item, nodes.If):
            alt = self._list(item.else_block.contents, nxt, brk)
            for elseif in reversed(item.elseifs):
                p = _LjdfixPoint('read', elseif, ('expression',))
                p.succ = [(self._list(elseif.then_block.contents, nxt, brk), ()), (alt, ())]
                alt = p
            p = _LjdfixPoint('read', item, ('expression',))
            p.succ = [(self._list(item.then_block.contents, nxt, brk), ()), (alt, ())]
            return p

        if isinstance(item, nodes.While):
            p = _LjdfixPoint('read', item, ('expression',))
            p.succ = [(self._list(item.statements.contents, p, nxt), ()), (nxt, ())]
            return p

        if isinstance(item, nodes.RepeatUntil):
            p = _LjdfixPoint('read', item, ('expression',))
            body = self._list(item.statements.contents, p, nxt)
            p.succ = [(body, ()), (nxt, ())]
            return body

        if isinstance(item, nodes.NumericFor):
            head = _LjdfixPoint('kill', [item.variable])
            again = _LjdfixPoint('nop')
            head.succ = [(self._list(item.statements.contents, again, nxt), ())]
            again.succ = [(head, ()), (nxt, ())]
            p = _LjdfixPoint('read', item, ('expressions',))
            p.succ = [(head, ()), (nxt, ())]
            return p

        if isinstance(item, nodes.IteratorFor):
            head = _LjdfixPoint('kill', item.identifiers.contents)
            head.succ = [(self._list(item.statements.contents, head, nxt), ()), (nxt, ())]
            p = _LjdfixPoint('read', item, ('expressions',))
            p.succ = [(head, ())]
            return p

        if isinstance(item, nodes.Break):
            p = _LjdfixPoint('nop')
            p.succ = [(brk, ())]
            return p

        p = _LjdfixPoint('stmt', item)
        self.points[item] = p
        if not isinstance(item, nodes.Return):
            p.succ = [(nxt, ())]
        return p


def _ljdfix_flow(fn):
    if fn is None:
        return None
    flow = _LJDFIX_FLOWS.get(fn)
    if flow is None:
        flow = _LJDFIX_FLOWS[fn] = _LjdfixFlow(fn)
    return flow


def _ljdfix_unaccounted_read(flow, start, slot, accounted):
    # First read of slot reachable from after `start` with no write of slot on the way
    # that is not in accounted -> (point, identifier or None, what it is), or None.
    # Anything the flow can't follow counts as such a read.
    work = list(start.succ)
    seen = set()
    while work:
        point, edge_kills = work.pop()
        if point is None or slot in edge_kills or point in seen:
            continue
        seen.add(point)
        if point.kind == 'stale':
            return point, None, 'stale warp target'
        roots, writes = point.effect(slot)
        here = point.what if point.kind == 'stmt' else None
        for ident in _ljdfix_reads(roots, slot, flow, here):
            if ident is _LJDFIX_UNKNOWN:
                return point, None, 'unknown node type'
            if ident is _LJDFIX_CLOSURE:
                return point, None, 'closure capture'
            if ident not in accounted:
                return point, ident, 'read'
        if not writes:
            work.extend(point.succ)
    return None


def _ljdfix_key(ref):
    # The key a constructor ref will have when it is folded: its own key, or, for a key
    # still in a register, what that register's definition earlier in the same block will
    # be inlined as. Only an integer Constant can make insert_table_record refuse, and
    # ljd folds no constants, so anything else is returned as-is. None if unknown (the
    # register is set from another register, or not in this block).
    key = ref.path[-2].key
    if not (isinstance(key, nodes.Identifier) and key.type == nodes.Identifier.T_SLOT):
        return key
    statement = ref.path[-4]
    block = next((n for n in reversed(ref.path) if isinstance(n, nodes.Block)), None)
    if block is None or statement not in block.contents:
        return None
    for item in reversed(block.contents[:block.contents.index(statement)]):
        if isinstance(item, nodes.Assignment) and _ljdfix_writes(item.destinations.contents, key.slot):
            if len(item.destinations.contents) != 1 or len(item.expressions.contents) != 1:
                return None
            key = item.expressions.contents[0]
            if isinstance(key, nodes.Identifier) and key.type == nodes.Identifier.T_SLOT:
                return None
            return key
    return None


def _ljdfix_folds(constructor, refs):
    # Whether _eliminate_into_table_constructors will fold every one of refs into
    # constructor: helpers.insert_table_record(..., replace=False) refuses only an
    # integer key inside the array part whose element isn't nil. Mirrors it step by step.
    def is_nil(value):
        return isinstance(value, nodes.Primitive) and value.type == nodes.Primitive.T_NIL

    array = [is_nil(record.value) for record in constructor.array.contents]
    unknown = False  # a slot key seen earlier, which a later inline may make an integer
    for ref in refs:
        key = _ljdfix_key(ref)
        value = ref.path[-4].expressions.contents[0]
        if key is None:
            # Folded as whatever it is inlined to by then: refused if that is an integer
            # hitting a non-nil element, so assume the worst once anything is non-nil.
            if unknown or not all(array):
                return False
            unknown = True
            continue
        if not isinstance(key, nodes.Constant) or key.type != key.T_INTEGER or key.value < 0:
            continue
        if unknown:
            return False
        index = key.value
        if index == 1 and not array:
            array.append(True)
        if index > len(array):
            continue
        if not array or index == len(array):
            array.append(is_nil(value))
        elif array[index]:
            array[index] = is_nil(value)
        else:
            return False
    return True


def _ljdfix_blocked(info, accounted, tag='VETO', ref=None):
    # Whether deleting info's assignment would lose a value some read still needs.
    # Fails closed: an assignment the check can't place is kept.
    assignment = info.assignment
    _LJDFIX_STATS['checked'] += 1

    # The collector knows the function the assignment is in when that function starts
    # inside the collection's root; otherwise it is the root's own function.
    root = getattr(info, '_ljdfix_root', None)
    root_fn = _ljdfix_function_of(root)
    fn = info.function or root_fn
    flow = _ljdfix_flow(fn)
    point = flow.points.get(assignment) if flow is not None else None

    exempt = False
    if isinstance(root, nodes.Block):
        # The collection may be rooted in an enclosing function (it descends into
        # nested ones), so look the root up in its own function's flow.
        root_flow = _ljdfix_flow(root_fn)
        # unwarper.py:570's temporary block: the blocks it is made of are deleted right
        # after this call, so keeping the assignment would not keep the value. Only for
        # the root's own function: a function nested in those blocks survives them.
        exempt = fn is root_fn and (root_flow is None or root not in root_flow.blocks)

    found = None
    if point is None:
        found = None, None, 'assignment not in the flow'
        _LJDFIX_STATS['unlocated'] += 1
    elif ref is not None and _ljdfix_andor_value(flow, info, ref):
        _LJDFIX_STATS['andor'] += 1
    else:
        found = _ljdfix_unaccounted_read(flow, point, info.slot, accounted)
    if found is None and point is not None:
        # `slotA, slotB = nil` (KNIL) goes through the simple path, and eliminating it
        # deletes every destination: no other destination may be read either.
        for dst in assignment.destinations.contents:
            if isinstance(dst, nodes.Identifier) and dst.type == nodes.Identifier.T_SLOT \
                    and dst.slot != info.slot:
                found = _ljdfix_unaccounted_read(flow, point, dst.slot, ())
                if found is not None:
                    break
    if found is None:
        tables = getattr(info, '_ljdfix_tables', None)
        src = assignment.expressions.contents[0]
        if tables and isinstance(src, nodes.TableConstructor) and not _ljdfix_folds(src, tables):
            # A constructor ref that won't fold stays as `slotN[k] = v`: it needs slotN.
            found = point, None, 'table element that will not fold'
    if found is None:
        return False
    if exempt:
        _LJDFIX_STATS['exempt'] += 1
        _ljdfix_log('EXEMPT', info, fn, found)
        return False
    _LJDFIX_STATS['vetoed'] += 1
    _ljdfix_log(tag, info, fn, found)
    return True


def _ljdfix_vetoed(info, ref):
    if not isinstance(info.assignment, nodes.Assignment):
        return False
    accounted = getattr(info, '_ljdfix_accounted', None)
    return _ljdfix_blocked(info, accounted if accounted is not None else {ref.identifier}, ref=ref)


def _ljdfix_andor_value(flow, info, ref):
    # One part of a value-producing and/or: `slotN = <part>` ends its block, and the
    # block's warp tests slotN itself (IST/ISF). Its jump carries the part's value to the
    # expression's end, the fall-through goes on to the next part, and every other part
    # writes slotN before it gets there. ljd represents that value as the warp's
    # condition: the expression unwarper builds `slotN = a and <part> or b` from the
    # conditions, and refuses a non-start part block that holds an assignment as well
    # (unwarper.py:899-903). So the inline into the test is how ljd keeps the value, and
    # keeping the assignment breaks the expression (`c and f() or d` came out as an if
    # that drops `d`). Returns whether this is such a part.
    assignment = info.assignment
    block = next((b for b in flow.blocks if b.contents and b.contents[-1] is assignment), None)
    if block is None:
        return False
    warp = block.warp
    if not isinstance(warp, nodes.ConditionalWarp) or getattr(warp, '_ljdfix_test', None) != info.slot:
        return False
    cond = warp.condition
    if isinstance(cond, nodes.UnaryOperator) and cond.type == nodes.UnaryOperator.T_NOT:
        cond = cond.operand
    if cond is not ref.identifier:
        return False

    # Every path from the fall-through must write slotN before it reads slotN or reaches
    # the jump target, through blocks shaped like expression parts: at most one
    # statement, and that one a write of slotN.
    join = warp.false_target
    stack, seen = [warp.true_target], set()
    while stack:
        b = stack.pop()
        if b is join or b is None or b not in flow.blocks:
            return False
        if b in seen:
            continue
        seen.add(b)
        contents = [c for c in b.contents if not _is_invalidated(c)]
        if len(contents) > 1:
            return False
        if contents:
            c = contents[0]
            if isinstance(c, nodes.Assignment):
                if len(c.destinations.contents) != 1 or not _ljdfix_writes(c.destinations.contents, info.slot):
                    return False
                if any(True for _ in _ljdfix_reads(c.expressions.contents, info.slot, flow, c)):
                    return False
                continue
            if not isinstance(c, nodes.NoOp):
                return False
        w = b.warp
        if isinstance(w, nodes.ConditionalWarp):
            if any(True for _ in _ljdfix_reads([w.condition], info.slot, flow, None)):
                return False
            stack.append(w.true_target)
            if getattr(w, '_ljdfix_copy', None) != info.slot:
                stack.append(w.false_target)
        elif isinstance(w, nodes.UnconditionalWarp):
            stack.append(w.target)
        else:
            return False
    return True


def _ljdfix_text(node, depth=0):
    # A short rendering of an expression, for the log only.
    if depth > 4:
        return '...'
    if isinstance(node, nodes.Identifier):
        if node.type == nodes.Identifier.T_SLOT:
            return 'slot%d' % node.slot
        return str(node.name)
    if isinstance(node, nodes.Constant):
        return repr(node.value)
    if isinstance(node, nodes.Primitive):
        return ('nil', 'true', 'false')[node.type]
    if isinstance(node, nodes.TableElement):
        key = node.key
        if isinstance(key, nodes.Constant) and key.type == nodes.Constant.T_STRING:
            return '%s.%s' % (_ljdfix_text(node.table, depth + 1), key.value)
        return '%s[%s]' % (_ljdfix_text(node.table, depth + 1), _ljdfix_text(key, depth + 1))
    if isinstance(node, nodes.FunctionCall):
        args = ', '.join(_ljdfix_text(a, depth + 1) for a in node.arguments.contents)
        func = _ljdfix_text(node.function, depth + 1)
        if node.is_method and '.' in func:
            head, _, tail = func.rpartition('.')
            func = head + ':' + tail
        return '%s(%s)' % (func, args)
    if isinstance(node, nodes.BinaryOperator):
        return '(%s op%d %s)' % (_ljdfix_text(node.left, depth + 1), node.type, _ljdfix_text(node.right, depth + 1))
    return type(node).__name__


def _ljdfix_name(fn):
    # The name the function is stored under, looked up now (after the main pass it is
    # `_env.NAME = function`; during it, often only `slotN = function`).
    if fn is None:
        return '-'
    parent = getattr(fn, '_ljdfix_fn', None)
    if parent is None:
        return '<main>'
    stack = [parent.statements]
    while stack:
        node = stack.pop()
        items = getattr(node, 'contents', None)
        if isinstance(node, (nodes.Block, nodes.StatementsList)):
            for i, item in enumerate(items):
                if not isinstance(item, nodes.Assignment):
                    continue
                for dst, expr in zip(item.destinations.contents, item.expressions.contents):
                    if expr is not fn:
                        continue
                    if isinstance(dst, nodes.Identifier) and dst.type == nodes.Identifier.T_SLOT:
                        # `slotN = function ... end` then `_env.NAME = slotN`
                        for later in items[i + 1:]:
                            if isinstance(later, nodes.Assignment) and len(later.expressions.contents) == 1:
                                e = later.expressions.contents[0]
                                if isinstance(e, nodes.Identifier) and e.type == e.T_SLOT and e.slot == dst.slot:
                                    return _ljdfix_text(later.destinations.contents[0])
                    return _ljdfix_text(dst)
        if not isinstance(node, (nodes.FunctionDefinition, nodes.Identifier)):
            stack.extend(_ljdfix_children(node))
    return '?'


def _ljdfix_log(tag, info, fn, found):
    if not _LJDFIX_LOG:
        return
    frame = sys._getframe(1)
    while frame is not None and os.path.basename(frame.f_code.co_filename) in ('slotworks.py', 'ljd_slotworks_fix.py'):
        frame = frame.f_back
    caller = '%s:%d' % (os.path.basename(frame.f_code.co_filename), frame.f_lineno) if frame else '?'
    assignment = info.assignment
    src = assignment.expressions.contents[0] if assignment.expressions.contents else None
    where = '-'
    if found is not None:
        point, _, why = found
        what = point.what if point is not None else None
        where = '%s %s/%s@%s' % (why, point.kind if point is not None else '-', type(what).__name__,
                                 getattr(what, '_addr', '?'))
    with open(_LJDFIX_LOG, 'a', encoding='utf-8', newline='\n') as f:
        f.write('%s\tcaller=%s\tfn=%s\tslot%d#%d\tdef@%s\t%s = %s\tread=%s\n' % (
            tag, caller, _ljdfix_name(fn), info.slot, info.slot_id, getattr(assignment, '_addr', '?'),
            'slot%d' % info.slot, _ljdfix_text(src), where))


def eliminate_temporary(ast, ignore_ambiguous=True, identify_slots=False, safe_mode=True, unwarped=False):
    # ljd_fixed: the identify_slots pass (main.py:380) is the one that sees the whole file.
    if identify_slots:
        _ljdfix_stamp(ast)

    _eliminate_multres(ast)

    slots, unused = _collect_slots(ast, identify_slots=identify_slots, unwarped=unwarped)
    _sort_slots(slots)
    _eliminate_temporary(ast, slots, ignore_ambiguous, safe_mode=safe_mode, unwarped=unwarped)

    # _remove_unused(unused)

    if not unwarped:
        _cleanup_invalid_nodes(ast)

    return ast


def _collect_slots(ast, identify_slots=False, unwarped=False):
    collector = _SlotsCollector(identify_slots, unwarped)
    traverse.traverse(collector, ast)

    # ljd_fixed: what each slot was collected over; _ljdfix_vetoed needs it (the
    # recheck collects over one block, not over eliminate_temporary's root).
    for info in collector.slots:
        info._ljdfix_root = ast

    return collector.slots, collector.unused


def _fill_simple_refs(info, data: RefsProcessData, ignore_ambiguous, safe_mode=True):
    src = info.assignment.expressions.contents[0]

    src_is_table = isinstance(src, nodes.TableConstructor)

    holders = set()

    # Collect all the simple refs as we go through, then make a policy decision about what to
    # do with them at the end. This also folds in the table constructor elements, even if simple
    # inlining cannot be performed (though mutator.py would likely pick this up regardless).
    new_simple = []

    # Check if we've had a single non-table-constructor-write reference yet. If so, none of the
    # following references can be part of the constructor.
    # Without this, the following:
    #
    # local test = {}
    # f(test)
    # test.abc = "hi"
    #
    # Decompiles to:
    #
    # slot0 = { abc = "hi" }
    # f(slot0)
    #
    # Note that when debugging this, it may be wise to disable mutator.py - it can interfere
    # with the results and move things into the constructor that otherwise wouldn't be.
    all_ctor_refs = True

    # ljd_fixed: the references this policy consumes (inlines into, or folds into the
    # constructor). Any other read of the value - one this collection could not see,
    # skipped as ambiguous, or dropped as a duplicate holder below - makes _ljdfix_vetoed
    # keep the assignment.
    accounted = set()
    info._ljdfix_tables = []

    for ref in info.references[1:]:
        if ignore_ambiguous and ref.identifier.id == -1:
            continue

        holder = _get_holder(ref.path)

        is_element = isinstance(holder, nodes.TableElement)

        if is_element:
            # Fixes an error on this:
            # local a = tbl[var or 123]:func()
            # This is due to the compiler only evaluating (var or 123) once, then mov-ing that to
            # another slot, and this results in two different slot references that have the same holder.
            if holder in holders:
                continue

            holders.add(holder)

        accounted.add(ref.identifier)
        path_index = ref.path.index(holder)

        statement = _get_holder(ref.path[:path_index])

        statement_is_assignment = isinstance(statement, nodes.Assignment)

        if statement_is_assignment:
            is_dst = statement.destinations.contents[0] == holder
        else:
            is_dst = False

        if debug_verify:
            for tst_info, tst_ref, _ in new_simple:
                if tst_info == info:
                    tst_holder = tst_ref.path[-2]
                    assert tst_holder != ref.path[-2]
                    assert tst_holder != holder

        # Could be more then one reference here
        if src_is_table and is_element and is_dst and all_ctor_refs:
            assert holder.table == ref.identifier
            data.tables.append((info, ref))
            info._ljdfix_tables.append(ref)
        else:
            new_simple.append((info, ref, None))
            all_ctor_refs = False

    info._ljdfix_accounted = accounted

    # Don't attempt to simplify any slots that have more than two usages (excluding table constructor elements).
    # This is a major policy change, as slotworks used to inline almost anything and
    # everything, with the exception of the results of function calls with more than three
    # uses.
    # This caused a LOT of incorrect decompilation results, however this is only noticeable
    # when running against stripped code - when a slot has a name attached, it cannot be
    # simplified and thus many of these issues would not appear.
    # For an example of the issues this caused, see issue #19 (https://gitlab.com/znixian/luajit-decompiler/issues/19)
    # Also note that we only inline the use if it's still valid at the end.
    #
    # And as for the issue 19 problem, why isn't that still happening like this? There's still
    # only one simple reference, since the other one has been moved into the tables system. The
    # answer is 55b2f5c, which introduced all_ctor_refs. Since the table is referenced before the
    # third use, it cannot be moved into the constructor (well, mutator will move it in, but it'll
    # be safe from being eliminated)
    nr_simple_cases = len(new_simple)
    if nr_simple_cases == 1 or (not safe_mode and nr_simple_cases == 2):
        data.simple += new_simple
    elif nr_simple_cases > 1:
        data.unsafe.append(info)


def _eliminate_simple_cases(simple):
    # ljd_fixed: flows are built lazily for this batch. Inside it the tree only changes
    # by the eliminations below, which _LjdfixFlow reads through; between batches
    # anything may have changed.
    _LJDFIX_FLOWS.clear()

    for info, ref, src in simple:
        holder = ref.path[-2]
        dst = ref.identifier

        if src is None:
            src = info.assignment.expressions.contents[0]

        # Set later on if we'd be simplifying something down to illegal Lua code. This is when we
        # substitute a primitive to somewhere it's not allowed, such as:
        #    local a=nil; a()  ---> nil()
        # We're doing this here rather than in _fill_simple_refs, since that doesn't actually know what
        # it's substituting - a primitive could work it's way down a bunch of slots and it'd only know about
        # the primitive when eliminating the first slot.
        # See the illegal_type_eliminations test for an example of this
        if isinstance(src, nodes.Primitive) or isinstance(src, nodes.Constant):
            is_str = isinstance(src, nodes.Constant) and src.type == nodes.Constant.T_STRING
            if isinstance(holder, nodes.FunctionCall) and holder.function == ref.identifier:
                continue
            elif isinstance(holder, nodes.TableElement) and holder.table == ref.identifier and not is_str:
                continue

        # if the assignment's src is FunctionDefinition and references 3 times(one time for assignment,and two
        # times for call),so marked the identifier to local type and set the name to tmp slot
        # TODO figure out *why* the functions are ending up here and fix it there
        if isinstance(src, nodes.FunctionDefinition) and len(info.references) >= 3:
            # Make sure it's actually 3 references, ignoring the ambiguous ones
            nr_references = 0
            for _ref in info.references:
                if _ref.identifier.id == -1 and len(getattr(_ref, "_ids", [])) != 1:
                    continue
                nr_references += 1
            if nr_references >= 3:
                first = info.references[0]
                first.identifier.type = nodes.Identifier.T_LOCAL
                if first.identifier.name is None:
                    first.identifier.name = 'slot%d' % first.identifier.slot
                continue

        # ljd_fixed: keep the assignment if some other read still needs its value. This
        # sits before the operator special case below, which edits the call it inlines into.
        if _ljdfix_vetoed(info, ref):
            continue

        if isinstance(src, OPERATOR_TYPES) \
                and isinstance(holder, nodes.TableElement) \
                and holder.key == dst \
                and isinstance(ref.path[-3], nodes.FunctionCall):
            # Handle a special case where a function has been incorrectly marked as a method now that
            # a slot will be reduced to an expression with an operator
            function = ref.path[-3]
            if function.is_method and \
                    (not isinstance(function, nodes.TableElement)
                     or function.key.type != nodes.Constant.T_STRING):
                function.arguments.contents.insert(0, holder.table)
                function.is_method = False

        _mark_invalidated(info.assignment)

        if isinstance(holder, LIST_TYPES):
            conts = holder.contents
            found = _replace_node_in_list(conts, dst, src)
        else:
            found = _replace_node(holder, dst, src)

        assert found

        # ljd_fixed: every place this value has been computed, oldest first (see _ljdfix_reads).
        src._ljdfix_origins = getattr(src, '_ljdfix_origins', ()) + (info.assignment,)


def _eliminate_mass_assignments(massive):
    # ljd_fixed: _fill_massive_refs's _remove_invalid_references drops trailing id == -1
    # references, so a value read again after a branch can look like a one-use copy here
    # too. Flows are rebuilt for this batch, as in _eliminate_simple_cases.
    _LJDFIX_FLOWS.clear()

    for identifier, info, base_assignment, globalvar in massive:
        # If the assignment using the slot has already been invalidated, then skip it.
        # For example, the following:
        #
        # local a, b = f()
        # print(a)
        #
        # Would be (roughly) compiled to:
        #
        # 001 local slot1, slot2 = f()
        # 002 local slot3 = slot1
        # 003 print(slot3)
        #
        # Both the massives elimination and simples elimination would eliminate 002 (corresponding
        # to the base_assignment variable in this method), moving slot3 into the massive assignment.
        # The simple elimination would also eliminate slot3, substituting slot1 into the print directly.
        # This would result in:
        #
        # local slot3, slot2 = f()
        # print(slot1)
        #
        # Since this is run after the simple elimination, check to ensure our target hasn't been swept
        # out from under us.
        if _is_invalidated(base_assignment):
            continue

        # ljd_fixed: renaming the destination deletes this slot's write; keep it if any read
        # but the copy's still needs it.
        if _ljdfix_blocked(info, {ref.identifier for ref in info.references[1:]}, 'VETO-MASS'):
            for ident in info._ljdfix_removed:
                ident._ids.append(info.slot_id)
                ident._ids.sort()
            continue

        destinations = info.assignment.destinations.contents
        found = _replace_node_in_list(destinations, identifier, globalvar)

        _mark_invalidated(base_assignment)

        assert found


def _fill_massive_refs(info, data: RefsProcessData, ignore_ambiguous, safe_mode=True):
    ref = info.references[1]
    holder = _get_holder(ref.path)
    info._ljdfix_removed = []

    src = info.assignment.expressions.contents[0]

    # When the compiler needs to null out multiple variables, it can use
    # the KNIL instruction to do so. For example, the following:
    #   local a = nil; local b = nil; local c = nil;
    # Compiles down to a single KNIL instruction (assuming a-c are slots 0-2):
    #   KNIL 0 2
    # There's no point in treating it like the result of a function call or
    # vararg, since (AFAIK) the point of massive refs for those is since they
    # can't really be inlined around because they're all tied together. That's
    # not an issue with KNIL (since all the values are independently nil), so
    # we can just hand over to the simple ref filler, which will (or should, may
    # need to look into it) inline it as usual.
    #
    # In particular, this solves the massive_nils test crashing since the results
    # are referenced more than twice.
    if isinstance(src, nodes.Primitive):
        assert src.type == src.T_NIL

        _fill_simple_refs(info, data, ignore_ambiguous, safe_mode)
        return

    def _remove_invalid_references():
        if not safe_mode:
            return
        # TODO need to check why these invalid references end up here and whether they're actually invalid
        while len(info.references) > 2:
            current_ref = info.references[-1].identifier
            if current_ref.id != -1:
                break
            possible_ids = getattr(current_ref, "_ids", [])
            possible_ids.remove(info.slot_id)
            # ljd_fixed: remembered so a vetoed rename can put it back (see
            # _eliminate_mass_assignments); a later pass removes it again.
            info._ljdfix_removed.append(current_ref)
            del info.references[-1]

    assert isinstance(src, (nodes.FunctionCall, nodes.Vararg))
    if isinstance(holder, nodes.Assignment):
        dst = holder.destinations.contents[0]

        _remove_invalid_references()

        # It's perfectly valid for there to be >2 references. This does mean we can't eliminate them as
        # massive references. Instead, leave them as a local variable.
        if len(info.references) != 2:
            data.unsafe.append(info)
            return

        orig = info.references[0].identifier

        assignment = ref.path[-3]

        assert isinstance(assignment, nodes.Assignment)

        data.massive.append((orig, info, assignment, dst))
    elif isinstance(holder, nodes.IteratorWarp):
        _remove_invalid_references()
        assert len(info.references) == 2
        data.iterators.append((info, src, holder))
