#include <stdio.h>
#include <windows.h>

#define CODEPAGE_KEY "SYSTEM\\CurrentControlSet\\Control\\Nls\\CodePage"
#define BACKUP_KEY "SOFTWARE\\UTF8CompatibilityFix"

int is_admin(void) {
	BOOL isAdmin = FALSE;
	PSID adminGroup = NULL;
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

	if (AllocateAndInitializeSid(
		&ntAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&adminGroup
	)) {
		CheckTokenMembership(NULL, adminGroup, &isAdmin);
		FreeSid(adminGroup);
	}

	return isAdmin;
}

int read_value(HKEY root, const char *keyPath, const char *name, char *buffer, DWORD size) {
	return RegGetValueA(
		root,
		keyPath,
		name,
		RRF_RT_REG_SZ,
		NULL,
		buffer,
		&size
	) == ERROR_SUCCESS;
}

int write_value(HKEY root, const char *keyPath, const char *name, const char *value) {
	HKEY key;

	if (RegCreateKeyExA(
		root,
		keyPath,
		0,
		NULL,
		0,
		KEY_SET_VALUE,
		NULL,
		&key,
		NULL
	) != ERROR_SUCCESS) {
		return 0;
	}

	LONG result = RegSetValueExA(
		key,
		name,
		0,
		REG_SZ,
		(const BYTE *)value,
		(DWORD)(strlen(value) + 1)
	);

	RegCloseKey(key);

	return result == ERROR_SUCCESS;
}

int backup_exists(void) {
	char value[32];

	return read_value(
		HKEY_LOCAL_MACHINE,
		BACKUP_KEY,
		"ACP",
		value,
		sizeof(value)
	);
}

int backup_encoding(void) {
	char acp[32];
	char oemcp[32];
	char maccp[32];

	if (backup_exists()) {
		return 1;
	}

	if (!read_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "ACP", acp, sizeof(acp)) ||
		!read_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "OEMCP", oemcp, sizeof(oemcp)) ||
		!read_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "MACCP", maccp, sizeof(maccp))) {
		return 0;
	}

	return write_value(HKEY_LOCAL_MACHINE, BACKUP_KEY, "ACP", acp) &&
		write_value(HKEY_LOCAL_MACHINE, BACKUP_KEY, "OEMCP", oemcp) &&
		write_value(HKEY_LOCAL_MACHINE, BACKUP_KEY, "MACCP", maccp);
}

void enable_utf8(void) {
	if (!backup_encoding()) {
		printf("\nFailed to back up the original system encoding.\n");
		return;
	}

	if (!write_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "ACP", "65001") ||
		!write_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "OEMCP", "65001") ||
		!write_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "MACCP", "65001")) {
		printf("\nFailed to enable UTF-8 compatibility mode.\n");
		return;
	}

	printf("\nUTF-8 compatibility mode has been enabled.\n");
	printf("Restart Windows to apply the changes.\n");
}

void restore_encoding(void) {
	char acp[32];
	char oemcp[32];
	char maccp[32];

	if (!read_value(HKEY_LOCAL_MACHINE, BACKUP_KEY, "ACP", acp, sizeof(acp)) ||
		!read_value(HKEY_LOCAL_MACHINE, BACKUP_KEY, "OEMCP", oemcp, sizeof(oemcp)) ||
		!read_value(HKEY_LOCAL_MACHINE, BACKUP_KEY, "MACCP", maccp, sizeof(maccp))) {
		printf("\nNo original encoding backup was found.\n");
		return;
	}

	if (!write_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "ACP", acp) ||
		!write_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "OEMCP", oemcp) ||
		!write_value(HKEY_LOCAL_MACHINE, CODEPAGE_KEY, "MACCP", maccp)) {
		printf("\nFailed to restore the original system encoding.\n");
		return;
	}

	RegDeleteTreeA(HKEY_LOCAL_MACHINE, BACKUP_KEY);

	printf("\nOriginal system encoding has been restored.\n");
	printf("Restart Windows to apply the changes.\n");
}

int main(void) {
	int choice;

	if (!is_admin()) {
		printf("This program must be run as Administrator.\n");
		system("pause");
		return 1;
	}

	printf("========================================\n");
	printf("Program Name: UTF-8 Compatibility Fix\n");
	printf("Author: SimSool\n");
	printf("Version: 1.0.0\n");
	printf("Github: https://github.com/SILENCE-SIMSOOL\n");
	printf("========================================\n\n");

	printf("[1] Enable UTF-8 Compatibility Mode\n");
	printf("[2] Restore Original System Encoding\n\n");

	printf("Select an option: ");
	scanf("%d", &choice);

	switch (choice) {
	case 1:
		enable_utf8();
		break;

	case 2:
		restore_encoding();
		break;

	default:
		printf("\nInvalid option.\n");
		break;
	}

	printf("\n");
	system("pause");

	return 0;
}