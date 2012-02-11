
#ifndef BANK_H
#define BANK_H

#include "Definitions.h"
#include "Program.h"
#include "XML/ticpp.h"

using namespace ticpp;

class ModuleData;
class CtrlInfo;


typedef vector< Program* > ProgramList;

class Bank : public ProgramList
{
public:
    Bank();
	~Bank();

    void clear();
    void close( bool doSave );

	void load( const string& path, bool append=false );
    void save( const string& path, bool saveCurrent, bool backup=true );
    void newBank( const string& path, const string& name, bool save );

	Program* setProgram( INT32 programNum, bool save );
    Program* getCurrentProgram() { return &currentProgram_; }
    void setCurrentProgram( Program* program );
	static Program* generateDefaultProgram( INT32 programNum );
    static Program* generateEmptyProgram( INT32 programNum );
    void generateDefaultBank();
    Program* getProgram( INT32 programNum );
    
    void getProgramName( INT32 programNum, char* name );
    void insertProgram( INT32 programNum, Program* program );
    void removeProgram( INT32 programNum, bool withDelete=true );
    void saveProgram( INT32 programNum, Program* program );
    void saveCurrentProgram( INT32 programNum );

    static void getXml( ModuleData* data, ostringstream& result );
    static void getXml( Program* program, ostringstream& result );
    static void createFromXml( istringstream& is, Program& program );
    static void createFromXml( istringstream& is, ProgramList& list );

    string path_;
    string name_;
    INT32 programNum_;

protected:
    static void readProgram( const Document& doc, Program* program );
	static void readProgram( Element* element, Program* program );
    static Element* writeProgram( Program* program );

	static void readModule( Element* element, ModuleData* data );
    static Element* writeModule( ModuleData* data );
	static void readParams( Element* element, ModuleData* data );
    //static void readCtrlInfo( Iterator< Element > it, CtrlInfo& ctrlInfo );
    static void readParamData( Iterator< Element > it, ParamData* paramData );
    //static void readParamData( Iterator< Element > it, ParamDataMap& params );
    static void writeParams( Element* moduleElement, ModuleData* data );
    //static void writeCtrlInfo( Element* param, CtrlInfo& ctrlInfo );
    static void writeParamData( Element* param, ParamData& paramData, CtrlInfo* defaultInfo );
	static void readLinks( Element* element, ModuleData* data );
    static void writeLinks( Element* moduleElement, ModuleData* data );

    void copyProgram( const Program& source, INT32 programNum );
    void saveCurrentProgram();
    void makeBackup();
    
	Program currentProgram_;
    static const char* defaultProgramXml_;
};

#endif // BANK_H