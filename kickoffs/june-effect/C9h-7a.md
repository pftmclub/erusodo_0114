# June effect completion - C9h-7a - June's sound-device auto-release, ported verbatim

| | |
|---|---|
| Start | new conversation on **Sonnet 5.5, high** effort, then type `/x2-phase june-effect C9h-7a` |
| Plan doc | `JUNE2017_EFFECT_COMPLETION_PHASES.md` |
| Depends on | C9h-7 (done: map, census, audit; the user chose **(b) verbatim**, 2026-10-08) |
| Flag | `SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE` (+ `SERV_IRUHADEV_JUNE_SOUND_AUTO_RELEASE_DEBUG`, on for the play-test) |
| Log tag | `JUNE_FX SOUND_RELEASE` (`python tools/logs.py --grep "JUNE_FX SOUND_RELEASE"`) |
| Review | `kickoffs/june-effect/C9h-7a-review.md` (write it when done) |

## Read first (and nothing else from the plan doc unless you need it)

```sh
python tools/docsection.py JUNE2017_EFFECT_COMPLETION_PHASES.md "C9h-7 - Done"
grep -n "Effect C9h-7" june2017_re/funcmap.tsv
cat june2017_re/c9h7_playsound_handle_audit.tsv
awk -F'\t' '$3 !~ /^1/' june2017_re/c9h7_playsound_census.tsv      # the 28 sites that do not pass 1
grep -n "^| 12[25] " JUNE_BUGS.md
```

March: `KTDXLIB/KTDXDeviceManager.cpp` (`PlaySound` :1100, `OnFrameMove` :125, `CloseDevice` :256), `KTDXLIB/KTDXDevice.cpp` (`UnrefDevice` :180), `KTDXLIB/KTDXDeviceSound.h/.cpp` (`m_pChannel`, the compiled-out `FMODChannelCallback` :5 / :607). Several of these files are CP949, so edit them with `tools/bytepatch.py`.

## Task

Port June's auto-release **verbatim** (user decision (b), 2026-10-08). The map is in C9h-7's Done note and funcmap rows. Port it as June has it:

1. **`CKTDXDeviceManager::PlaySound( name, bLoop, b3DSound, <4th> )`** `31ce0e10`. The 4th parameter defaults to **true** (June's majority; June has no recoverable default, so this is our choice, recorded in C9h-7). Name it `bJuneAutoRelease`, labelled as ours.
   - In the **open** branch only, after `Play`, register the channel-end callback on the sound's `m_pChannel`. This is `31cdd4e0`; add a flag-gated getter on `CKTDXDeviceSound`.
   - The **found** branch stays March's: no reference taken, no callback.
   - Check the FMOD C API's `FMOD_Channel_SetCallback` signature in the vendored FMOD header before you use it. March's only call is compiled out, so it has never been compiled. Also check June's IAT import `007a0458`, so you call the same function June does.
2. **The callback** `31cdcca0`: on `END`, call the queue function `31cdcbb0` on `g_pKTDXApp->GetDeviceManager()`, then return `FMOD_OK`.
   - The queue function, under `m_DeviceLock`: the first device in the map with type `DT_SOUND` whose `m_pChannel` equals the ended channel is pushed into a new vector. That is June's `+0x240`; `break` after the push.
3. **The drain**, in `CKTDXDeviceManager::OnFrameMove` `31cde8d0`: after `ProcessForegroundQueue`, close each queued device, then clear the list.
   - The close follows June's `CloseDevice( CKTDXDevice* )` `31cdd8d0`: if the count is > 1, one compare-exchange decrement and return when it took. Otherwise, under the lock, decrement; at zero, find the device by ID, erase it and delete it.
   - March's `CloseDevice( name )` does the same unref. Reuse it only if the result is identical, and say which one you chose.
   - Leave `31c4a0d0( this+0x108 )` (unidentified) out and record that in the Done note.
4. **Per-site arguments: June's, verbatim.**
   - Pass **0** at the March counterparts of June's 24 zero sites. 16 are in the audit `.tsv`: create-unit x9, `PlaySoundSelect`, the TFieldNpc near sound, the wedding duration sound, the emblem, and the two `KTDXApp` Lua wrappers.
   - Find the March counterparts of the dropped-handle zero sites: `X2_Button_Mouse_Up` (`324c7ce0`), `Count` and `LevelUpBGM` (`3250f680`), `Go` (`327f4210`), `NewStage_Open` (`3293b5e0`), `UseItem` (`32952930`).
   - Settle the 4 unresolved registers by reading their functions: `3250f680` `ebp` x3, `32e2b400` `ebx`.
   - Every other site keeps the default. **The square and Add keep June's 1 and June's bug** (JUNE_BUGS 122 / 125: no fix, no guard).
   - No class file needs an edit. If one turns out to, it goes to the character-port checklist.
5. **Debug log** (`_DEBUG`, throttled): `JUNE_FX SOUND_RELEASE queue <name>` and `... close <name> ref=<n> deleted=<0|1>`.

**Done gate:**
- Strings in the deployed exe.
- A play-test run whose log shows `queue` / `close` lines with `deleted=1` for a non-preloaded sound.
- 0 EXCEPTION, and no new crash bundle in `%TEMP%`.
- Note any load hitch: a non-preloaded sound is reloaded on every play now.
- **Do not fix the Add / square use-after-free.** If a play-test crashes there, stop and report it to the user with the crash bundle: that is the "copying June breaks March" case.
