# AnoCrypto-C agent instructions

- Confirm the authorized GitHub issue and scope before editing. Use [Current / Active / Next](docs/roadmap/CURRENT_ACTIVE_NEXT.md) for product priorities and the [development workflow](docs/roadmap/DEVELOPMENT_WORKFLOW.md#agent-assisted-delivery) for delegation, worktree isolation, review, and evidence.
- Work on a dedicated branch in an isolated worktree with a separate build directory. Preserve other branches and worktrees. Link the draft PR to its issue and report checks against the reviewed revision.
- Escalate changes to cryptographic behavior, lifecycle or public contracts, validation, security policy, or release boundaries. Keep credentials, secret material, and customer data out of the repository and review evidence.
- The maintainer controls merge and scope approval. Follow the [release authorization gate](docs/release/RELEASE_PROVENANCE_AND_SIGNING_POLICY.md#external-release-gate). External release remains `EXTERNAL_RELEASE_NOT_AUTHORIZED`.
