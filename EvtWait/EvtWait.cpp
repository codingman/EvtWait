// EvtWait.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <winevt.h>

HANDLE hEvent2 = NULL;
#pragma comment(lib, "wevtapi.lib")
DWORD WINAPI SubscriptionCallback(EVT_SUBSCRIBE_NOTIFY_ACTION action, PVOID pContext, EVT_HANDLE hEvent) {
	printf(" ... Fired ...%d \n", (DWORD)action );
	SetEvent(hEvent2);
	return 1;

}

WCHAR Query[1000];

int main( int argc, TCHAR *argv [])
{
	DWORD status = ERROR_SUCCESS;
	EVT_HANDLE hSubscription = NULL;
	LPWSTR pwsQuery = L"<QueryList><Query Id=\"0\" Path=\"%S\"><Select Path=\"%S\">*[System[Provider[ @Name='%S'] and (EventID=%S)]]</Select></Query></QueryList>";


	if (argc != 4) {
		printf("Usage:\n%s EventlogName Source EventId\n\nExample: %s \"Operations Manager\" \"Health Service Script\" 1000\n ", argv[0], argv[0]);
		return 0;
	}
	swprintf(Query, sizeof(Query)/sizeof(WCHAR), pwsQuery, argv[1], argv[1], argv[2], argv[3]);


	printf("%d %S\n", argc, Query);
	hEvent2 = CreateEvent(NULL, FALSE, FALSE, NULL);
	hSubscription = EvtSubscribe(NULL, NULL  , L"", Query, NULL, NULL,
		(EVT_SUBSCRIBE_CALLBACK)SubscriptionCallback, EvtSubscribeToFutureEvents);
	if (NULL == hSubscription)
	{
		status = GetLastError();

		if (ERROR_EVT_CHANNEL_NOT_FOUND == status)
			wprintf(L"Channel  was not found.\n");
		else if (ERROR_EVT_INVALID_QUERY == status)
			// You can call EvtGetExtendedStatus to get information as to why the query is not valid.
			wprintf(L"The query \"%s\" is not valid.\n", Query);
		else
			wprintf(L"EvtSubscribe failed with %lu.\n", status);

		return 0;
	}
	else
		WaitForSingleObject(hEvent2, INFINITE);



	if (hSubscription)
		EvtClose(hSubscription);
}

