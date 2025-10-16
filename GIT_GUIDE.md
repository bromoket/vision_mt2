# Git CLI Guide for Vision MT2

## 🚀 Initial Setup

### Configure Git Identity
```bash
git config --global user.name "bromoket"
git config --global user.email "your-email@example.com"
```

### Check Configuration
```bash
git config --list
git config user.name
git config user.email
```

## 📦 Repository Setup

### Initialize New Repository
```bash
git init
```

### Clone Existing Repository
```bash
git clone https://github.com/bromoket/vision_mt2.git
cd vision_mt2
```

### Add Remote Origin
```bash
git remote add origin https://github.com/bromoket/vision_mt2.git
git remote -v  # Verify remotes
```

## 📝 Basic Workflow

### Check Status
```bash
git status                    # See current status
git status --short           # Compact status
```

### Add Files
```bash
git add .                    # Add all files
git add *.cpp                # Add specific pattern
git add CMakeLists.txt       # Add specific file
git add vision_external/     # Add directory
```

### Commit Changes
```bash
git commit -m "Initial commit"
git commit -m "Add new feature: memory management"
git commit -m "Fix: resolve static linking issues"
git commit -m "Update: enhance Visual Studio organization"
```

### View History
```bash
git log                      # Full history
git log --oneline           # Compact history
git log --graph --oneline   # Visual graph
git log -5                  # Last 5 commits
```

## 🌐 Remote Operations

### Push to Remote
```bash
git push origin main         # Push to main branch
git push -u origin main     # Set upstream and push
git push --all              # Push all branches
git push --tags             # Push tags
```

### Fetch from Remote
```bash
git fetch                   # Fetch all branches
git fetch origin            # Fetch from origin
git fetch origin main      # Fetch specific branch
```

### Pull from Remote
```bash
git pull                    # Fetch + merge current branch
git pull origin main       # Pull specific branch
git pull --rebase          # Pull with rebase instead of merge
```

## 🌿 Branch Management

### Create and Switch Branches
```bash
git branch feature-ui       # Create branch
git checkout feature-ui     # Switch to branch
git checkout -b feature-ui  # Create and switch
git switch feature-ui       # Modern switch command
git switch -c feature-ui    # Create and switch (modern)
```

### List Branches
```bash
git branch                  # Local branches
git branch -r              # Remote branches
git branch -a              # All branches
```

### Delete Branches
```bash
git branch -d feature-ui    # Delete merged branch
git branch -D feature-ui    # Force delete branch
git push origin --delete feature-ui  # Delete remote branch
```

## 🔄 Merging

### Merge Branches
```bash
git checkout main           # Switch to target branch
git merge feature-ui        # Merge feature branch
git merge --no-ff feature-ui  # Force merge commit
```

### Resolve Merge Conflicts
```bash
# 1. Edit conflicted files manually
# 2. Mark as resolved:
git add conflicted-file.cpp
# 3. Complete merge:
git commit
```

## 🔧 Advanced Operations

### Stash Changes
```bash
git stash                   # Stash current changes
git stash push -m "WIP: UI improvements"  # Stash with message
git stash list             # List stashes
git stash pop              # Apply and remove latest stash
git stash apply stash@{1}  # Apply specific stash
git stash drop stash@{1}   # Delete specific stash
```

### Reset and Revert
```bash
git reset HEAD~1           # Undo last commit (keep changes)
git reset --hard HEAD~1    # Undo last commit (discard changes)
git revert HEAD            # Create new commit that undoes last commit
git checkout -- file.cpp  # Discard changes to file
```

### Rebase
```bash
git rebase main            # Rebase current branch onto main
git rebase -i HEAD~3       # Interactive rebase last 3 commits
```

## 🏷️ Tags

### Create Tags
```bash
git tag v1.0.0             # Lightweight tag
git tag -a v1.0.0 -m "Version 1.0.0 - Initial release"  # Annotated tag
```

### List and Push Tags
```bash
git tag                    # List tags
git show v1.0.0           # Show tag info
git push origin v1.0.0    # Push specific tag
git push --tags           # Push all tags
```

## 🔍 Inspection Commands

### Diff Changes
```bash
git diff                   # Working directory vs staging
git diff --staged         # Staging vs last commit
git diff HEAD~1           # Current vs previous commit
git diff main..feature-ui # Compare branches
```

### Show Commit Details
```bash
git show HEAD             # Show last commit
git show v1.0.0          # Show specific commit/tag
git show --stat HEAD     # Show files changed
```

### Blame and History
```bash
git blame file.cpp        # See who changed each line
git log --follow file.cpp # History of specific file
git log -p file.cpp       # Show patches for file
```

## 🚨 Emergency Commands

### Undo Last Commit (Not Pushed)
```bash
git reset --soft HEAD~1   # Keep changes staged
git reset HEAD~1          # Keep changes unstaged
git reset --hard HEAD~1   # Discard changes completely
```

### Fix Last Commit Message
```bash
git commit --amend -m "Corrected commit message"
```

### Recover Lost Commits
```bash
git reflog                # Show reference log
git checkout <commit-hash>  # Recover lost commit
```

## 📋 Vision MT2 Specific Workflow

### Initial Setup for Vision MT2
```bash
# 1. Configure git
git config user.name "bromoket"
git config user.email "your-email@example.com"

# 2. Add all files
git add .

# 3. Initial commit
git commit -m "Initial commit: Vision MT2 framework"

# 4. Create GitHub repository (via web interface)
# 5. Add remote and push
git remote add origin https://github.com/bromoket/vision_mt2.git
git push -u origin main
```

### Development Workflow
```bash
# 1. Create feature branch
git checkout -b feature/new-hooks

# 2. Make changes to vision_internal/src/hooks/
# 3. Test with build scripts
./build.ps1

# 4. Commit changes
git add .
git commit -m "Add new game hooks for memory scanning"

# 5. Push feature branch
git push -u origin feature/new-hooks

# 6. Create pull request via GitHub web interface
# 7. After review, merge to main
git checkout main
git pull origin main
git branch -d feature/new-hooks
```

### Release Workflow
```bash
# 1. Update version in CMakeLists.txt
# 2. Build and test
./build-production.ps1

# 3. Commit version bump
git add CMakeLists.txt
git commit -m "Bump version to v1.1.0"

# 4. Create and push tag
git tag -a v1.1.0 -m "Release v1.1.0 - Enhanced UI and new features"
git push origin main
git push origin v1.1.0
```

## 🔗 Useful Aliases

Add these to your git config for faster commands:
```bash
git config --global alias.st status
git config --global alias.co checkout
git config --global alias.br branch
git config --global alias.ci commit
git config --global alias.unstage 'reset HEAD --'
git config --global alias.last 'log -1 HEAD'
git config --global alias.visual '!gitk'
git config --global alias.graph 'log --graph --oneline --all'
```

## 📚 Quick Reference

| Command | Description |
|---------|-------------|
| `git status` | Check working directory status |
| `git add .` | Stage all changes |
| `git commit -m "message"` | Commit with message |
| `git push` | Push to remote |
| `git pull` | Pull from remote |
| `git checkout -b branch` | Create and switch branch |
| `git merge branch` | Merge branch |
| `git log --oneline` | View commit history |
| `git diff` | Show changes |
| `git stash` | Temporarily save changes |

## 🆘 Getting Help

```bash
git help <command>        # Detailed help for command
git <command> --help     # Same as above
git help -a              # List all commands
```

---

**Pro Tips:**
- Always `git status` before committing
- Use meaningful commit messages
- Create branches for features
- Pull before pushing
- Use `.gitignore` to exclude build files
- Commit often, push when stable
