- **Avoid hard-coding**: Do not hard-code values.
- **Do not add obvious comments** that do not bring any value. Only add comments where necessary.
```
// Bad - comment by Captain Obvious:
// Parse metadata
parseMetadata(doc)

// Good - comment that explains an important implementation detail:
// Showing the first item while the requested item is still loading
items.firstOrNull() ?: emptyItem
```
- **Avoid generating duplicated code** - refactor and reuse instead.
