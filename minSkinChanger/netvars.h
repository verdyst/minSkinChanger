#pragma once
#include <Windows.h>

class RecvTable;

class RecvProp
{
	// This info comes from the receive data table.
public:


	const char* m_pVarName;
	void* m_RecvType;
	int						m_Flags;
	int						m_StringBufferSize;
	bool					m_bInsideArray;		// Set to true by the engine if this property sits inside an array.

	// Extra data that certain special property types bind to the property here.
	const void* m_pExtraData;

	// If this is an array (DPT_Array).
	RecvProp* m_pArrayProp;
	void* m_ArrayLengthProxy;

	void* m_ProxyFn;
	void* m_DataTableProxyFn;	// For RDT_DataTable.

	RecvTable* m_pDataTable;		// For RDT_DataTable.
	int						m_Offset;

	int						m_ElementStride;
	int						m_nElements;

	// If it's one of the numbered "000", "001", etc properties in an array, then
	// these can be used to get its array property name for debugging.
	const char* m_pParentArrayPropName;
};


class RecvTable
{
public:

	// Properties described in a table.
	RecvProp* m_pProps;
	int				m_nProps;

	// The decoder. NOTE: this covers each RecvTable AND all its children (ie: its children
	// will have their own decoders that include props for all their children).
	void* m_pDecoder;

	const char* m_pNetTableName;	// The name matched between client and server.


	bool			m_bInitialized;
	bool			m_bInMainList;
};

class ClientClass;
// Linked list of all known client classes
extern ClientClass* g_pClientClassHead;

// The serial number that gets passed in is used for ehandles.
typedef void* (*CreateClientClassFn)(int entnum, int serialNum);
typedef void* (*CreateEventFn)();

//-----------------------------------------------------------------------------
// Purpose: Client side class definition
//-----------------------------------------------------------------------------
class ClientClass
{

public:
	CreateClientClassFn		m_pCreateFn;
	CreateEventFn			m_pCreateEventFn;	// Only called for event objects.
	const char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int						m_ClassID;	// Managed by the engine.
};

intptr_t GetOffset(RecvTable* table, const char* tablename, const char* netvarname)
{

	for (int i = 0; i < table->m_nProps; i++)
	{
		RecvProp prop = table->m_pProps[i];

		if (!_stricmp(prop.m_pVarName, netvarname))
		{
			return prop.m_Offset;
		}

		if (prop.m_pDataTable)
		{
			intptr_t offset = GetOffset(prop.m_pDataTable, tablename, netvarname);

			if (offset)
			{
				//std::cout << netvarname << " " << prop.m_pVarName << " : offset : " << offset << " prop.m_Offset : " << prop.m_Offset << std::endl;
				return offset + prop.m_Offset;
			}
		}
	}

	return 0;
}

intptr_t GetNetVarOffset(const char* tablename, const char* netvarname, ClientClass* clientclass)
{
	ClientClass* currnode = clientclass;

	for (auto currnode = clientclass; currnode; currnode = currnode->m_pNext)
	{
		if (!_stricmp(tablename, currnode->m_pRecvTable->m_pNetTableName))
		{
			return GetOffset(currnode->m_pRecvTable, tablename, netvarname);
		}
	}

	return 0;
}

class IBaseClientDll
{
public:
	virtual void fn0() = 0;
	virtual void fn1() = 0;
	virtual void fn2() = 0;
	virtual void fn3() = 0;
	virtual void fn4() = 0;
	virtual void fn5() = 0;
	virtual void fn6() = 0;
	virtual void fn7() = 0;

	virtual ClientClass* GetAllClasses() = 0;

};