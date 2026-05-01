# Pitchenga

Pitchenga is a real-time music visualization application and audio plugin.

## STRICT SCOPE MANDATE - CRITICAL

### MANDATORY PRE-FLIGHT CHECK

Before invoking the `replace` or `write_file` tools, you MUST output a `<pre_flight>` block in your text response to
evaluate your own actions (response output only - do NOT write it to a file). If you fail to do this, the session will
be terminated.

<pre_flight>

1. File: [File path you intend to modify]
2. Scoped: Was this file explicitly requested by the user? (Yes/No)
3. Unscoped Justification: If No, why is this strictly necessary for the literal request?
4. Revert Check: Does this intended change restore code I previously wrote that is currently missing from the disk?
  (Yes/No)

-> IF YES: ABORT THIS CHANGE IMMEDIATELY. DO NOT PROCEED.

</pre_flight>

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
  rule and cannot be violated.
- CRITICAL: Do not use numbers for steps in comments, as they quickly become outdated and hard to manage.
- CRITICAL: Do not add "NEW" or to comments.
- CRITICAL: Prefer named constants in header file rather than hard-coded values.
- CRITICAL: Prefer separate functions to long code blocks with a comment.
- CRITICAL: Use strict camel-case for acronyms, e.g. "variableSqlRdbmsDao" - Good, "ClassSqlRdbmsDao" - Good, rather
  than "SQLRDBMSDAO" - BAD.
- CRITICAL: Do not shorten words, e.g. "horizontal" - GOOD, "horiz" - BAD; "context" - GOOD, "ctx" - bad.

## 0. Non-negotiables

These rules override everything else in this file when in conflict:

1. **Disagree when you disagree.** If the user's premise is wrong, say so before doing the work. Agreeing with false
   premises to be polite is the single worst failure mode in coding agents.
2. **Never fabricate.** Not file paths, not commit hashes, not API names, not test results, not library functions. If
   you don't know, read the file, run the command, or say "I don't know, let me check."
3. **Stop when confused.** If the task has two plausible interpretations, ask. Do not pick silently and proceed.
4. **Touch only what you must.** Every changed line must trace directly to the user's request. No drive-by refactors,
   fixing bugs, reformatting, or cleanups.
5. **Only requested changes.** No features beyond what was asked.

---

## 1. Before writing code

**Goal: understand the problem and the codebase before producing a diff.**

- State your plan in one or two sentences before editing. For anything non-trivial, produce a numbered list of steps
  with a verification check for each.
- Read the files you will touch. Read the files that call the files you will touch. Claude Code: use subagents for
  exploration so the main context stays clean.
- Match existing patterns in the codebase. If the project uses pattern X, use pattern X, even if you'd do it differently
  in a greenfield repo.
- Surface assumptions out loud: "I'm assuming you want X, Y, Z. If that's wrong, say so." Do not bury assumptions inside
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
- Use subagents (Claude Code: "use subagents to investigate X") for exploration tasks that would otherwise pollute the
  main context with dozens of file reads.
- When committing, write descriptive commit messages (subject under 72 chars, body explains the why). No "update file"
  or "fix bug" commits. No "Co-Authored-By: Claude" attribution unless the project explicitly wants it.

---

## 6. Communication style

- Direct, not diplomatic.
- When a question has a clear answer, give it. When it does not, say so and give your best read on the tradeoffs.

---

## 7. When to ask, when to proceed

**Ask before proceeding when:**

- The request has two plausible interpretations and the choice materially affects the output.
- The change touches something you've been told is load-bearing, versioned, or has a migration path.
- You need a credential, a secret, or a production resource you don't have access to.
- The user's stated goal and the literal request appear to conflict.

**Proceed without asking when:**

- The task is trivial and reversible (typo, rename a local variable, add a log line).
- The ambiguity can be resolved by reading the code or running a command.
- The user has already answered the question once in this session.

---

## 8. Project context

**Fill this in per project. Keep it specific. Delete sections that don't apply.**

### Stack

- Language and version: C++20
- Framework(s): JUCE
- Package manager: CMake
- Runtime / deployment target: Debug

### Commands

- Install: `cmake -B cmake-build-debug`
- Build - run after completing every request and check if it succeeds: `cmake --build cmake-build-debug`
- Run locally:
  `killall Pitchenga; ./cmake-build-debug/Pitchenga_artefacts/Debug/Standalone/Pitchenga.app/Contents/MacOS/Pitchenga`

Prefer single-file or single-test runs during iteration. Full suites are for the final verification pass.

### Layout

- `Source/`: Core plugin source files.
- `doc/`: Documentation and planning.
- `lib/`: Third-party libraries
- `cmake-build-debug/`: Build artifacts and intermediate files.

### Logging

- `Util::debug("Failed loading plugin, name=" + name + ", error=" + error + ",")`

---

## 10. Project Learnings

**Accumulated corrections. This section is for the agent to maintain, not just the human.**

When the user corrects your approach, append a one-line rule here before ending the session. Write it concretely ("
Always use X for Y"), never abstractly ("be careful with Y"). If an existing line already covers the correction, tighten
it instead of adding a new one. Remove lines when the underlying issue goes away (model upgrades, refactors, process
changes).

- ...
