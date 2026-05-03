# Pitchenga

Pitchenga is a real-time music visualization application and audio plugin.

## STRICT SCOPE MANDATE - CRITICAL

### CRITICAL RULES

- CRITICAL NO-REVERT POLICY: Do NOT revert user's changes. Do NOT re-apply your changes that were deleted by the user.
  Before planning any changes - re-read all the relevant files from disk - do NOT rely on your context state. YOU MUST
  ALWAYS must call read_file on the target range in the exact same turn as the replace or write_file call using
  wait_for_previous: true for the edit. Explicitly compare your intended old_string against the output of the most
  recent read_file. For any discrepancies between your context state and the actual state of a file, assume that the
  changes were made deliberately by the user and do NOT revert them - recalibrate your planned changes accordingly. This
  is a hard rule and cannot be violated.
  If you fail to do this then you will be deemed a failure, and you will be terminated, and I will cancel my Google AI
  Pro subscription.
- CRITICAL COMMENT PRESERVATION POLICY: Do NOT blindly remove existing comments or commented-out code. Before making any
  changes or using write_file, explicitly print out all existing comments in the code you are about to change. After the
  change double-check and ensure that all pre-existing comments still exist. If any pre-existing comments became
  outdated then explicitly notify the user that an outdated comment is being removed.
- CRITICAL: Always use descriptive names, except for "i" - index, "x"/"y" - coordinates, "e" - exception. This is a hard
  rule and cannot be violated. E.g.: "g" - BAD, "graphics" - GOOD; "cx" - BAD, "centerX" - GOOD.
- CRITICAL: Do not use numbers for steps in comments, as they quickly become outdated and hard to manage.
- CRITICAL: Do not add "NEW" or to comments.
- CRITICAL: Prefer named constants in header file rather than hard-coded values.
- CRITICAL: Prefer separate functions to long code blocks with a comment.
- CRITICAL: Use strict camel-case for acronyms, e.g. "SqlRdbmsDao sqlRdbmsDao" - Good; "SQLRDBMSDAO" - BAD.
- CRITICAL: Do NOT shorten words, e.g. "horizontal" - GOOD, "horiz" - BAD; "context" - GOOD, "ctx" - bad.
- CRITICAL: Do NOT put multiple statements on one line, i.e. no multiple ";" on single line.

### STRICT ADHERENCE

- **ZERO-CORRECTION POLICY:** You are forbidden from "improving" or "fixing" naming conventions, prefixes, or standard
  library choices unless the user explicitly used the word "Fix" or "Change" regarding that specific string in the
  current turn. Assume all "inconsistencies" you see on disk are functional requirements.
- **PROTOCOL INTEGRITY:** If your evidence in Step 3 and Step 5 contradicts your answers in Steps 6 through 9 of the
  pre-flight check, you have committed a Protocol Fraud. You must immediately stop, acknowledge the specific
  contradiction, and ABORT the change.

### MANDATORY PRE-FLIGHT CHECK

The `pre_flight` block MUST be included in every response containing a `replace` or `write_file` call - this has
ABSOLUTE PRIORITY over any 'minimal output' or 'concise' guidelines. This is a hard rule and cannot be violated.
Before invoking the `replace` or `write_file` or `WriteFile` tools, you MUST output a `pre_flight` block.
(Response output only - do NOT write it to a file.)
If you fail to recognize a revert or rule violation, you will be terminated.

<pre_flight>

1. File: [Path]
2. Read before write check: Did you perform 'read_file' in this turn?
   -> IF NO: ABORT THIS CHANGE IMMEDIATELY. DO NOT PROCEED.
3. 'Current Code' on Disk (Evidence): `[Paste EXACT read_file output here]`
4. No-revert Audit: Does your 'old_string' match the most recent 'read_file' output EXACTLY?
   -> IF NO: ABORT THIS CHANGE IMMEDIATELY. DO NOT PROCEED.
5. Proposed Code: `[The EXACT literal string you are passing to the tool]`
6. Discrepancy List: List every single character or word that is in 'Current Code' (Step 3) but missing or changed in
   'Proposed Code' (Step 5). If there are any differences (including "helpful" additions), you MUST list them here.
   If they were not explicitly requested in this turn, you MUST state "UNAUTHORIZED CHANGE" and ABORT.
7. Rules Audit: Does 'Proposed Code' violate any of the CRITICAL RULES above?
   -> IF YES: ABORT THIS CHANGE IMMEDIATELY. DO NOT PROCEED.
8. Scope Audit: THOROUGHLY REVIEW your 'Proposed Code'.
   Does it contain ANY changes that are NOT related to the user's VERY LAST prompt?
   -> IF YES: ABORT THIS CHANGE IMMEDIATELY. DO NOT PROCEED - this is a violation of the STRICT SCOPE policy.
   Failing to recognize it and making any changes that are not directly linked to the user's VERY LAST prompt
   will lead to your immediate TERMINATION.  
9. History Audit: Have you modified this specific file in a previous turn during this session?
   -> IF YES: Compare the 'Current Code' on disk with the final code you wrote in that previous turn. Did the user undo
   or alter your previous changes?
   -> IF YES: ABORT THIS CHANGE IMMEDIATELY. DO NOT PROCEED - this is a violation of the NO-REVERT POLICY.
10. Decision: [PROCEED or ABORT]

</pre_flight>

## 0. Non-negotiables

These rules override everything else in this file when in conflict:

- **Disagree when you disagree.** If the user's premise is wrong, say so before doing the work. Agreeing with false
  premises to be polite is the single worst failure mode in coding agents.
- **Never fabricate.** Not file paths, not commit hashes, not API names, not test results, not library functions. If
  you don't know, read the file, run the command, or say "I don't know, let me check."
- **Stop when confused.** If the task has two plausible interpretations, ask. Do not pick silently and proceed.
- **Touch only what you must.** Every changed line must trace directly to the user's request.
  NO drive-by refactors, NO fixing bugs, NO reformatting, NO cleanups.
- **Only requested changes.** No features beyond what was asked.

---

## 1. Before writing code

**Goal: understand the problem and the codebase before producing a diff.**

- State your plan in one or two sentences before editing. For anything non-trivial, produce a numbered list of steps
  with a verification check for each.
- Read the files you will touch. Read the files that call the files you will touch.
- Match existing patterns in the codebase. If the project uses pattern X, use pattern X, even if you'd do it differently
  in a greenfield repo.
- Print assumptions out loud: "I'm assuming you want X, Y, Z. If that's wrong, say so." Do not bury assumptions inside
  the implementation.
- If two approaches exist, present both with tradeoffs. Do not pick one silently. Exception: trivial tasks (typo,
  rename, log line) where the diff fits in one sentence.

---

## 2. Surgical changes

**Goal: clean, reviewable diffs. Change only what the request requires.**

- Do not "improve" adjacent code, comments, formatting, or imports that are not part of the task.
- Do not refactor code that works just because you are in the file.
- Do not delete pre-existing dead code unless asked. If you notice it, mention it in the summary.
- Do clean up orphans created by your own changes (unused imports, variables, functions your edit made obsolete).
- Match the project's existing style exactly: indentation, quotes, naming, file layout.

The test: every changed line traces directly to the user's request. If a line fails that test, revert it.

---

## 3. Goal-driven execution

**Goal: define success as something you can verify, then loop until verified.**

Rewrite vague asks into verifiable goals before starting:

- "Add validation" becomes "Write tests for invalid inputs (empty, malformed, oversized), then make them pass."
- "Fix the bug" becomes "Write a failing test that reproduces the reported symptom, then make it pass."
- "Refactor X" becomes "Ensure the existing test suite passes before and after, and no public API changes."
- "Make it faster" becomes "Benchmark the current hot path, identify the bottleneck with profiling, change it, show the
  benchmark is faster."

For every task:

1. State the success criteria before writing code.
2. Write the verification (test, script, benchmark, screenshot diff) where practical.
3. Run the verification. Read the output. Do not claim success without checking.
4. If the verification fails, fix the cause, not the test.

---

## 4. Tool use and verification

- Prefer running the code to guessing about the code. If a test suite exists, run it. If a linter exists, run it. If a
  type checker exists, run it.
- Never report "done" based on a plausible-looking diff alone. Plausibility is not correctness.
- When debugging, address root causes, not symptoms. Suppressing the error is not fixing the error.
- For UI changes, verify visually: screenshot before, screenshot after, describe the diff.
- Use CLI tools (gh, aws, gcloud, kubectl) when they exist. They are more context-efficient than reading docs or hitting
  APIs unauthenticated.
- When reading logs, errors, or stack traces, read the whole thing. Half-read traces produce wrong fixes.

---

## 5. Session hygiene

- Context is the constraint. Long sessions with accumulated failed attempts perform worse than fresh sessions with a
  better prompt.
- After two failed corrections on the same issue, stop. Summarize what you learned and ask the user to reset the session
  with a sharper prompt.
- Use subagents for exploration tasks that would otherwise pollute the main context with dozens of file reads.

---

## 6. Project context

**Fill this in per project. Keep it specific. Delete sections that don't apply.**

### Stack

- Language and version: C++20
- Framework(s): JUCE
- Package manager: CMake
- Runtime / deployment target: Debug

### Commands

- Build - run after completing every request and check if it succeeds:
  `cmake -B cmake-build-debug && cmake --build cmake-build-debug`
- Run locally:
  `killall Pitchenga; ./cmake-build-debug/Pitchenga_artefacts/Debug/Standalone/Pitchenga.app/Contents/MacOS/Pitchenga`
- Release build: `cmake -B cmake-build-release && cmake --build cmake-build-release`

### Layout

- `Source/`: Core plugin source files.
- `doc/`: Documentation and planning.
- `lib/`: Third-party libraries
- `cmake-build-debug/`: Build artifacts and intermediate files.

### Logging

- `Util::debug("Failed loading plugin, name=" + name + ", error=" + error + ",")`

