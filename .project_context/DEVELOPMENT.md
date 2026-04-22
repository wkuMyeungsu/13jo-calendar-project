# 🛠 Development & Conventions

## Build Commands
```powershell
mkdir build; cd build; cmake ..; cmake --build .
```

## Coding Rules
- **Naming**: `m_variableName` for members.
- **Qt Parents**: Always pass parent pointer for memory management.
- **Styling**: Use `StyleHelper.h` exclusively for CSS/QSS.
- **Database**: All persistence MUST go through `DatabaseManager`.
- **Documentation**: Any change to file structure or major logic MUST be reflected in `.project_context/` immediately.
