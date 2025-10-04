# Git Setup Script for Windows
# Configures Git with credential management for seamless authentication

param(
    [string]$UserName,
    [string]$UserEmail,
    [switch]$Help
)

if ($Help) {
    Write-Host "Git Setup Script" -ForegroundColor Cyan
    Write-Host "Usage: .\setup-git.ps1 [-UserName <name>] [-UserEmail <email>] [-Help]" -ForegroundColor White
    Write-Host ""
    Write-Host "Parameters:" -ForegroundColor Yellow
    Write-Host "  -UserName   Your Git username (will prompt if not provided)" -ForegroundColor White
    Write-Host "  -UserEmail  Your Git email (will prompt if not provided)" -ForegroundColor White
    Write-Host "  -Help       Show this help message" -ForegroundColor White
    Write-Host ""
    Write-Host "This script will:" -ForegroundColor Yellow
    Write-Host "  1. Configure Git user name and email" -ForegroundColor Gray
    Write-Host "  2. Set up Git Credential Manager for Windows" -ForegroundColor Gray
    Write-Host "  3. Configure credential caching" -ForegroundColor Gray
    Write-Host "  4. Set up SSH key (optional)" -ForegroundColor Gray
    Write-Host "  5. Initialize this repository if needed" -ForegroundColor Gray
    exit 0
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Git Setup for Vision Project" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Function to check if command exists
function Test-Command {
    param([string]$Command)
    try {
        Get-Command $Command -ErrorAction Stop | Out-Null
        return $true
    } catch {
        return $false
    }
}

# Function to write colored output
function Write-Status {
    param([string]$Message, [string]$Status = "INFO")
    switch ($Status) {
        "ERROR" { Write-Host "[ERROR] $Message" -ForegroundColor Red }
        "WARN"  { Write-Host "[WARN]  $Message" -ForegroundColor Yellow }
        "OK"    { Write-Host "[OK]    $Message" -ForegroundColor Green }
        default { Write-Host "[INFO]  $Message" -ForegroundColor Cyan }
    }
}

# Check if Git is installed
if (-not (Test-Command "git")) {
    Write-Status "Git not found in PATH!" "ERROR"
    Write-Status "Please install Git for Windows from: https://git-scm.com/download/win" "ERROR"
    exit 1
}

$gitVersion = git --version
Write-Status "Found Git: $gitVersion" "OK"

# Get user information if not provided
if (-not $UserName) {
    $currentUser = git config --global user.name 2>$null
    if ($currentUser) {
        Write-Status "Current Git user: $currentUser"
        $response = Read-Host "Keep current user name? (y/n)"
        if ($response -eq "y" -or $response -eq "Y" -or $response -eq "") {
            $UserName = $currentUser
        }
    }
    
    if (-not $UserName) {
        $UserName = Read-Host "Enter your Git username"
    }
}

if (-not $UserEmail) {
    $currentEmail = git config --global user.email 2>$null
    if ($currentEmail) {
        Write-Status "Current Git email: $currentEmail"
        $response = Read-Host "Keep current email? (y/n)"
        if ($response -eq "y" -or $response -eq "Y" -or $response -eq "") {
            $UserEmail = $currentEmail
        }
    }
    
    if (-not $UserEmail) {
        $UserEmail = Read-Host "Enter your Git email"
    }
}

# Configure Git user
Write-Status "Configuring Git user information..."
git config --global user.name "$UserName"
git config --global user.email "$UserEmail"
Write-Status "Git user configured: $UserName <$UserEmail>" "OK"

# Configure Git Credential Manager
Write-Status "Configuring Git Credential Manager..."

# Check if Git Credential Manager is available
$gcmPath = Get-Command "git-credential-manager-core" -ErrorAction SilentlyContinue
if (-not $gcmPath) {
    $gcmPath = Get-Command "git-credential-manager" -ErrorAction SilentlyContinue
}

if ($gcmPath) {
    git config --global credential.helper manager-core
    Write-Status "Git Credential Manager configured" "OK"
} else {
    Write-Status "Git Credential Manager not found, using built-in credential helper" "WARN"
    git config --global credential.helper store
    Write-Status "Credential store configured (credentials will be saved in plain text)" "WARN"
}

# Configure credential caching
git config --global credential.helper 'cache --timeout=86400'  # 24 hours
Write-Status "Credential caching configured (24 hours)" "OK"

# Configure some useful Git settings
Write-Status "Configuring additional Git settings..."
git config --global init.defaultBranch main
git config --global pull.rebase false
git config --global core.autocrlf true
git config --global core.safecrlf false
git config --global push.default simple
Write-Status "Additional Git settings configured" "OK"

# SSH Key setup (optional)
Write-Host ""
$sshResponse = Read-Host "Do you want to set up SSH key authentication? (y/n)"
if ($sshResponse -eq "y" -or $sshResponse -eq "Y") {
    $sshDir = "$env:USERPROFILE\.ssh"
    $sshKeyPath = "$sshDir\id_rsa"
    
    if (-not (Test-Path $sshDir)) {
        New-Item -ItemType Directory -Path $sshDir | Out-Null
    }
    
    if (Test-Path $sshKeyPath) {
        Write-Status "SSH key already exists at $sshKeyPath" "OK"
        $overwrite = Read-Host "Overwrite existing key? (y/n)"
        if ($overwrite -ne "y" -and $overwrite -ne "Y") {
            Write-Status "Keeping existing SSH key" "OK"
        } else {
            ssh-keygen -t rsa -b 4096 -C "$UserEmail" -f $sshKeyPath -N '""'
            Write-Status "New SSH key generated" "OK"
        }
    } else {
        ssh-keygen -t rsa -b 4096 -C "$UserEmail" -f $sshKeyPath -N '""'
        Write-Status "SSH key generated at $sshKeyPath" "OK"
    }
    
    # Start SSH agent and add key
    if (Test-Command "ssh-agent") {
        Start-Service ssh-agent -ErrorAction SilentlyContinue
        ssh-add $sshKeyPath 2>$null
        Write-Status "SSH key added to agent" "OK"
    }
    
    # Display public key
    if (Test-Path "$sshKeyPath.pub") {
        Write-Host ""
        Write-Status "Your SSH public key:" "INFO"
        Write-Host "========================================" -ForegroundColor Yellow
        Get-Content "$sshKeyPath.pub"
        Write-Host "========================================" -ForegroundColor Yellow
        Write-Host ""
        Write-Status "Copy this key to your GitHub/GitLab account:" "INFO"
        Write-Status "GitHub: Settings > SSH and GPG keys > New SSH key" "INFO"
        Write-Status "GitLab: Preferences > SSH Keys > Add key" "INFO"
    }
}

# Initialize repository if not already a Git repo
if (-not (Test-Path ".git")) {
    Write-Host ""
    $initResponse = Read-Host "Initialize this directory as a Git repository? (y/n)"
    if ($initResponse -eq "y" -or $initResponse -eq "Y") {
        git init
        Write-Status "Git repository initialized" "OK"
        
        # Create initial commit
        if (Test-Path "README.md") {
            git add .
            git commit -m "Initial commit: Vision C++20 project scaffold"
            Write-Status "Initial commit created" "OK"
        }
    }
} else {
    Write-Status "Already a Git repository" "OK"
}

# Display current Git configuration
Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "Git Configuration Summary" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "User Configuration:" -ForegroundColor Yellow
Write-Host "  Name:  $(git config --global user.name)" -ForegroundColor White
Write-Host "  Email: $(git config --global user.email)" -ForegroundColor White
Write-Host ""
Write-Host "Credential Helper:" -ForegroundColor Yellow
Write-Host "  $(git config --global credential.helper)" -ForegroundColor White
Write-Host ""
Write-Host "Remote URLs (if configured):" -ForegroundColor Yellow
$remotes = git remote -v 2>$null
if ($remotes) {
    $remotes | ForEach-Object { Write-Host "  $_" -ForegroundColor White }
} else {
    Write-Host "  No remotes configured" -ForegroundColor Gray
}
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "  1. Add remote repository: git remote add origin <url>" -ForegroundColor Gray
Write-Host "  2. Push to remote: git push -u origin main" -ForegroundColor Gray
Write-Host "  3. Your credentials will be cached automatically" -ForegroundColor Gray
Write-Host ""
