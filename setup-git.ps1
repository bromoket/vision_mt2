#!/usr/bin/env pwsh

Write-Host "Setting up Git for Vision MT2..." -ForegroundColor Cyan
Write-Host ""

# Configure Git user
Write-Host "Configuring Git user..." -ForegroundColor Yellow
git config user.name "bromoket"
$email = Read-Host "Enter your email for Git commits"
git config user.email $email

Write-Host ""
Write-Host "Git configuration:" -ForegroundColor Green
git config user.name
git config user.email

Write-Host ""
Write-Host "Current repository status:" -ForegroundColor Yellow
git status --short

Write-Host ""
Write-Host "Ready to commit? (y/n)" -ForegroundColor Cyan
$commit = Read-Host

if ($commit -eq "y" -or $commit -eq "Y") {
    Write-Host "Adding all files..." -ForegroundColor Yellow
    git add .
    
    Write-Host "Creating initial commit..." -ForegroundColor Yellow
    git commit -m "Initial commit: Vision MT2 - Modern C++ game enhancement framework"
    
    Write-Host ""
    Write-Host "Commit created successfully!" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next steps:" -ForegroundColor Cyan
    Write-Host "1. Create repository on GitHub: https://github.com/new" -ForegroundColor White
    Write-Host "   - Repository name: vision_mt2" -ForegroundColor White
    Write-Host "   - Make it private if needed" -ForegroundColor White
    Write-Host ""
    Write-Host "2. Add remote and push:" -ForegroundColor White
    Write-Host "   git remote add origin https://github.com/bromoket/vision_mt2.git" -ForegroundColor Gray
    Write-Host "   git push -u origin main" -ForegroundColor Gray
    Write-Host ""
} else {
    Write-Host "Skipping commit. You can commit later with:" -ForegroundColor Yellow
    Write-Host "git add ." -ForegroundColor Gray
    Write-Host "git commit -m 'Initial commit'" -ForegroundColor Gray
}

Write-Host ""
Write-Host "Git setup complete! Check GIT_GUIDE.md for detailed commands." -ForegroundColor Green
Write-Host ""
Read-Host "Press Enter to continue"
