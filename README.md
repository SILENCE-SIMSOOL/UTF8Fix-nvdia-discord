# UTF-8 Compatibility Fix
A lightweight Windows command-line utility for enabling UTF-8 system locale compatibility and restoring the original system encoding later.
This tool is mainly intended to help resolve application launch issues related to Windows system encoding, including issues that may affect Discord or NVIDIA App.

## Features
- Enables Windows UTF-8 system locale compatibility
- Automatically backs up the original system encoding
- Restores the original encoding when needed
- Works across different Windows language/region configurations
- Simple command-line interface
- No installation required
- Lightweight and dependency-free

## Usage

Run the program as Administrator.

```text
[1] Enable UTF-8 Compatibility Mode
[2] Restore Original System Encoding
```

After applying either option, restart Windows for the changes to take effect.

## How It Works

When UTF-8 compatibility mode is enabled, the program first backs up the current Windows code page values:

```text
ACP
OEMCP
MACCP
```

The backup is stored in:

```text
HKEY_LOCAL_MACHINE\SOFTWARE\UTF8CompatibilityFix
```

Then the Windows code page values are changed to:

```text
65001
```

When restoring the original encoding, the program reads the previously saved values and restores them automatically.

Because the original values are backed up before any changes are made, the restore feature works across different Windows language and region configurations.

## Registry Location

The Windows system code page values are located at:

```text
HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Nls\CodePage
```

## Build

Using MinGW GCC:

```cmd
gcc -O2 -Wall UTF8Fix-nvdia-discord.c -o UTF8Fix-nvdia-discord.exe -ladvapi32
```

## Requirements
- Windows
- Administrator privileges
- System restart after applying changes

## Notes
This program modifies Windows system locale-related registry values.
The original values are backed up before UTF-8 mode is enabled, so they can be restored later using the restore option.

## License
This project is licensed under the MIT License.  
See the [LICENSE](https://github.com/SILENCE-SIMSOOL/UTF8Fix-nvdia-discord/blob/main/LICENSE) file for details.
