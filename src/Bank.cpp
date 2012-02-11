
#include "Bank.h"
#include "ModuleData.h"


const char* Bank::defaultProgramXml_ =
"<Program name=\"\" category=\"default\" comment=\"\" voices=\"32\">"
    "<Module id=\"0\" label=\"Master\" catalog=\"0\" poly=\"0\" x=\"440\" y=\"149\">"
        "<Param id=\"0\" value=\"0.35\" />"
        "<Link source=\"3\" out=\"0\" in=\"0\" value=\"1\" />"
    "</Module>"
    "<Module id=\"1\" label=\"Input\" catalog=\"3\" poly=\"1\" x=\"49\" y=\"148\" />"
    "<Module id=\"2\" label=\"Sine\" catalog=\"10\" poly=\"1\" x=\"185\" y=\"115\">"
        "<Param id=\"2\" value=\"0\" />"
        "<Param id=\"3\" value=\"0\"  />"
        "<Link source=\"1\" out=\"0\" in=\"0\" label=\"MidiFreq-&gt;Freq\" value=\"1\" />"
    "</Module>"
    "<Module id=\"3\" label=\"ADSR\" catalog=\"11\" poly=\"1\" x=\"319\" y=\"148\">"
        "<Param id=\"2\" value=\"0.0188976\" />"
        "<Param id=\"3\" value=\"0.204724\"  />"
        "<Param id=\"4\" value=\"0.590551\"  />"
        "<Param id=\"5\" value=\"2.8\"  />"
        "<Link source=\"1\" out=\"1\" in=\"1\" label=\"Gate\" value=\"1\" />"
        "<Link source=\"2\" out=\"0\" in=\"0\" label=\"Sine-&gt;In\" value=\"1\" />"
    "</Module>"
"</Program>";



//---------------------------------------------------------
// class Bank
//---------------------------------------------------------

Bank::Bank() :
    programNum_( 0 )
    {}


Bank::~Bank() 
{ 
    clear(); 
} 


void Bank::clear()
{
	currentProgram_.clear( true );

    for( UINT32 i=0; i<size(); i++ ) 
	{
		Program* program = at( i );
		if( program ) delete program;
	}
	ProgramList::clear();
}


void Bank::close( bool doSave )
{
    if( doSave ) {
        save( path_, true );
    }
    clear();
}


void Bank::load( const string& path, bool append )
{
    if( path.empty() ) return;

    if( append == false ) 
    {
		clear();	
        path_ = path;
	}

	try {
      	Document doc = Document( path );
        doc.LoadFile();

	    Element* bankElement = doc.FirstChildElement( "Bank", true );
        if( append == false )
        {
            bankElement->GetAttribute( "name",  &name_ );
            bankElement->GetAttribute( "program",  &programNum_ );
        }
	
	    Iterator< Element > it( "Program" );
	    for( it = it.begin( bankElement ); it != it.end(); it++ )
	    {
		    Element* programElement = it.Get();
		    Program* program        = new Program();
		
		    try {
			    readProgram( programElement, program );
                push_back( program );
		    }
		    catch( const exception& e ) {       // parse error, skip program
			    TRACE( e.what() );
		    }
	    }

        if( programNum_ >= (INT32)size() )
            programNum_ = 0;

        if( append == false && programNum_ < (INT32)size() )     // create current program
            currentProgram_ = *at( programNum_ );
	}
	catch( const exception& e )                 // file error, use default program
	{
        TRACE( e.what() );
        newBank( "New Bank.nexus", "New Bank", false );
	}
}


void Bank::save( const string& p, bool saveCurrent, bool backup )
{
    string path = p.empty() ? path_ : p;
    if( path.empty() ) 
        return;

    if( saveCurrent ) 
        saveCurrentProgram();

    if( backup )
        makeBackup();

    Document doc;
	Declaration* declaration = new Declaration(  "1.0", "", "no" );
	doc.LinkEndChild( declaration );

	Element* bankElement = new Element( "Bank" );
    bankElement->SetAttribute( "name", name_ );
    bankElement->SetAttribute( "program", programNum_ );
    	
	for( UINT32 i=0; i<size(); i++ )
	{
		Program* program = at( i );
        if( program->empty() == false )
        {
		    Element* programElement = writeProgram( program );
		    bankElement->LinkEndChild( programElement );
        }
	}
	doc.LinkEndChild( bankElement );

    try {
	    doc.SaveFile( path );
        path_ = path;
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
	doc.Clear();
}


void Bank::newBank( const string& path, const string& name, bool doSave )
{
    close( doSave );

    path_       = path;
    name_       = name;
    programNum_ = -1;

    Program* program = generateDefaultProgram( 0 );
    insert( end(), program );
    setProgram( 0, false );

    if( doSave )
        save( path_, false );
}


Program* Bank::setProgram( INT32 programNum, bool save )
{
    currentProgram_.clearModules( true );

    if( save || programNum == programNum_ ) 
        saveCurrentProgram();

    programNum_ = programNum;
    ASSERT( programNum_ >= 0 );

	Program* program = NULL;
    if( programNum_ >= (INT32)size() ) 
	{
        program = generateEmptyProgram( programNum_ );
        insert( end(), program );
	}
    else program = at( programNum_ );

    currentProgram_ = *program;
    return &currentProgram_;
}


void Bank::copyProgram( const Program& source, INT32 programNum )
{
    if( empty() == false ) {
        ASSERT( programNum_ >= 0 && programNum_ < (INT32)size() );
    }

    if( programNum_ >= 0 && programNum_ < (INT32)size() ) 
    {
        Program* program = at( programNum );
        *program         = source;
    }
}


void Bank::saveProgram( INT32 programNum, Program* program )
{
    if( programNum == programNum_ ) {
        saveCurrentProgram();
    }
}


void Bank::saveCurrentProgram()
{
    Program temp;

    temp = currentProgram_;
    temp.clearModules( false );

    copyProgram( temp, programNum_ );
}


void Bank::saveCurrentProgram( INT32 programNum )
{
    Program* program = new Program();

    *program = currentProgram_;
    program->clearModules( false );

    insertProgram( programNum, program );
}


void Bank::setCurrentProgram( Program* program )
{
    currentProgram_.clearModules( true );
    currentProgram_ = *program;
}


void Bank::getProgramName( INT32 programNum, char* name )
{
    Program* program = getProgram( programNum );

    if( program == NULL ) {
        strcpy_s( name, MAX_PROG_NAME_LEN, "[empty]" );
    }
    else {
        program->getName( name );
    }
}


Program* Bank::generateDefaultProgram( INT32 programNum )
{
	Program* program = NULL;
	try 
	{
    	Document doc;
	    doc.Parse( defaultProgramXml_, false );

		program = new Program();
		readProgram( doc, program );

		ostringstream os;
		os << programNum << " [Default]";
		program->name_ = os.str();
	}
	catch( const exception& e ) {
		TRACE( e.what() );
	}
	return program;
}


Program* Bank::generateEmptyProgram( INT32 programNum )
{
    Program* program = new Program;

    ostringstream os;
	os << programNum << " [Empty]";
	program->name_ = os.str();

    return program;
}


void Bank::generateDefaultBank()
{
    resize( NUMPROGRAMS );

    for( INT32 i=0; i<NUMPROGRAMS; i++ )
    {
        Program* program = generateDefaultProgram( i );
        insertProgram( i, program );
    }
    programNum_ = -1;
    name_       = "Default Bank";
    path_       = "";

    setProgram( 0, false );
}


Program* Bank::getProgram( INT32 programNum )
{
    if( programNum < 0 || programNum >= (INT32)size() ) {
        return NULL;
    }
    if( programNum == programNum_ ) {
        return &currentProgram_;
    }
    else return at( programNum );
}


void Bank::insertProgram( INT32 programNum, Program* program )
{
	ASSERT( program );
    programNum = min( (INT32)size(), programNum );

    insert( begin() + programNum, program );

    if( programNum <= programNum_ )  // if inserted before currentProgram, we must correct programNum
        programNum_++;
}


void Bank::removeProgram( INT32 programNum, bool withDelete )
{
    ASSERT( programNum >= 0 && programNum < (INT32)size() );

    if( programNum >= 0 && programNum < (INT32)size() ) 
    {
        Program* program = at( programNum );
        erase( begin() + programNum );

        if( withDelete )
            delete program;

        if( programNum == programNum_ ) {
        }
        else if( programNum_ > programNum ) {
            programNum_--;
            programNum_ = max( 0, programNum_ );
        }
    }
}


void Bank::readProgram( const Document& doc, Program* program )
{
	Element* programElement = doc.FirstChildElement( "Program", true );
	readProgram( programElement, program );
}


void Bank::readProgram( Element* element, Program* program )
{
	program->name_     = element->GetAttribute( "name" );
    program->category_ = element->GetAttribute( "category" );
    program->comment_  = element->GetAttribute( "comment" );

	element->GetAttribute( "voices", &program->numVoices_, false );
    element->GetAttribute( "unison", &program->numUnison_, false );
    element->GetAttribute( "sprd",   &program->unisonSpread_, false );
    element->GetAttribute( "hold",   &program->hold_, false );
    element->GetAttribute( "retrig", &program->retrigger_, false );
    element->GetAttribute( "legato", &program->legato_, false );

	Iterator< Element > it( "Module" );
	for( it = it.begin( element ); it != it.end(); it++ )
	{
		ModuleData* data = new ModuleData();
		Element* moduleElement = it.Get();
		readModule( moduleElement, data );

		program->addModule( data );
	}
}


Element* Bank::writeProgram( Program* program )
{
	Element* programElement = new Element( "Program" );

	programElement->SetAttribute( "name",     program->name_ );
    programElement->SetAttribute( "category", program->category_ );
    programElement->SetAttribute( "comment",  program->comment_ );
	programElement->SetAttribute( "voices",   program->numVoices_ );
    if( program->numUnison_ > 1 )
    {
        programElement->SetAttribute( "unison",   program->numUnison_ );
        programElement->SetAttribute( "sprd",     program->unisonSpread_ );
    }
    if( program->hold_ ) 
        programElement->SetAttribute( "hold", program->hold_ );
    if( program->retrigger_ ) 
        programElement->SetAttribute( "retrig", program->retrigger_ );
    if( program->legato_ ) 
        programElement->SetAttribute( "legato", program->legato_ );

	for( Program::iterator itProgram = program->begin(); itProgram != program->end(); itProgram++ ) 
	{
        Element* moduleElement = writeModule( *itProgram );
        programElement->LinkEndChild( moduleElement );
	}
	return programElement;
}


void Bank::readModule( Element* element, ModuleData* data )
{
	element->GetAttribute( "id",        &data->id_ );
    element->GetAttribute( "label",     &data->label_ );
    element->GetAttribute( "catalog",   (int*)&data->catalog_ );
	element->GetAttribute( "poly",      (int*)&data->polyphony_ );
	element->GetAttribute( "x",         &data->xPos_ );
	element->GetAttribute( "y",         &data->yPos_ );
    element->GetAttribute( "collapsed", &data->collapsed_, false );

	readParams( element, data );
	readLinks( element, data );
}


Element* Bank::writeModule( ModuleData* data )
{
	Element* element = new Element( "Module" );
	element->SetAttribute( "id",        data->id_ );
    element->SetAttribute( "label",     data->label_ );
    element->SetAttribute( "catalog",   data->catalog_ );
	element->SetAttribute( "poly",      data->polyphony_ );
	element->SetAttribute( "x",         data->xPos_ );
	element->SetAttribute( "y",         data->yPos_ );
    if( data->collapsed_ )
        element->SetAttribute( "collapsed", data->collapsed_ );

	writeParams( element, data );
	writeLinks( element, data );

    return element;
}


void Bank::readParams( Element* module, ModuleData* data )
{
	ParamDataMap params;
	params.makeDefaults( data->catalog_ );

	Iterator< Element > it( "Param" );
	for( it = it.begin( module ); it != it.end(); it++ )
	{
        UINT16 id;
        it->GetAttribute( "id", &id, false );

        ParamData* paramData = params.get( id );
        ASSERT( paramData );
        if( paramData == NULL ) continue;

        readParamData( it, paramData );
	}
	data->params_ = params;
}


void Bank::readParamData( Iterator< Element > it, ParamData* paramData )
{
    it->GetAttribute( "id",      &paramData->id_, false );
    it->GetAttribute( "value",   &paramData->value_, false );
    it->GetAttribute( "vsens",   &paramData->veloSens_, false );
    it->GetAttribute( "ktrack",  &paramData->keyTrack_, false );
    it->GetAttribute( "cc",      &paramData->controller_, false );
    it->GetAttribute( "ccmin",   &paramData->controllerMin_, false );
    it->GetAttribute( "ccmax",   &paramData->controllerMax_, false );
    it->GetAttribute( "ccsoft",  &paramData->controllerSoft_, false );
    it->GetAttribute( "style",   (int*)&paramData->style_, false );
    it->GetAttribute( "label",   &paramData->label_, false );
    it->GetAttribute( "unit",    &paramData->unit_, false );
    it->GetAttribute( "fmt",     (int*)&paramData->format_, false );
    it->GetAttribute( "dflt",    &paramData->defaultValue_, false );
    it->GetAttribute( "min",     &paramData->min_, false );
    it->GetAttribute( "max",     &paramData->max_, false );
    it->GetAttribute( "log",     &paramData->logFactor_, false );
    it->GetAttribute( "steps",   &paramData->numSteps_, false );
    it->GetAttribute( "res",     &paramData->resolution_, false );
}


void Bank::writeParamData( Element* param, ParamData& paramData, CtrlInfo* defaultInfo )
{
    param->SetAttribute( "value", paramData.value_ );

    if( paramData.label_ != defaultInfo->label_ )
        param->SetAttribute( "label", paramData.label_ );
    if( paramData.veloSens_ != 0 )
        param->SetAttribute( "vsens", paramData.veloSens_ );
    if( paramData.keyTrack_ != 0.f )
        param->SetAttribute( "ktrack", paramData.keyTrack_ );
    if( paramData.controller_ > -1 )
        param->SetAttribute( "cc", paramData.controller_ );
    if( paramData.controllerMin_ != 0.0f )
        param->SetAttribute( "ccmin", paramData.controllerMin_ );
    if( paramData.controllerMax_ != 127.0f )
        param->SetAttribute( "ccmax", paramData.controllerMax_ );
    if( paramData.controllerSoft_ != true )
        param->SetAttribute( "ccsoft", paramData.controllerSoft_ );
    if( paramData.style_ != defaultInfo->style_ )
        param->SetAttribute( "style", paramData.style_ );
    if( paramData.unit_ != defaultInfo->unit_ )
        param->SetAttribute( "unit",  paramData.unit_ );
    if( paramData.format_ != defaultInfo->format_ )
        param->SetAttribute( "fmt",   paramData.format_ );
    if( paramData.defaultValue_ != defaultInfo->defaultValue_ )
        param->SetAttribute( "dflt",  paramData.defaultValue_ );
    if( paramData.min_ != defaultInfo->min_ )
        param->SetAttribute( "min",   paramData.min_ );
    if( paramData.max_ != defaultInfo->max_ )
        param->SetAttribute( "max",   paramData.max_ );
    if( paramData.logFactor_ != defaultInfo->logFactor_ )
        param->SetAttribute( "log",   paramData.logFactor_ );
    if( paramData.numSteps_ != defaultInfo->numSteps_ )
        param->SetAttribute( "steps", paramData.numSteps_ );
    if( paramData.resolution_ != defaultInfo->resolution_ )
        param->SetAttribute( "res",   paramData.resolution_ );
}


void Bank::readLinks( Element* element, ModuleData* data )
{
	Iterator< Element > it( "Link" );
	for( it = it.begin( element ); it != it.end(); it++ )
	{
		LinkData linkData;
		it->GetAttribute( "source", &linkData.sourceId_ );
		it->GetAttribute( "in",     &linkData.inputId_ );
		it->GetAttribute( "out",    &linkData.outputId_ );

        readParamData( it, &linkData );

		data->addLink( linkData );
	}
}


void Bank::writeParams( Element* moduleElement, ModuleData* data )
{
	ParamDataMap* params = &data->params_;
	for( ParamDataMap::iterator itParams = params->begin(); itParams != params->end(); itParams++ ) 
	{
		Element* param       = new Element( "Param" );
        ParamData& paramData = itParams->second;
        
        if( paramData.style_ > CS_INVISIBLE ) 
        {
    		param->SetAttribute( "id",  paramData.id_ );

            CtrlInfo* defaultInfo = ModuleInfo::getParamInfo( data->catalog_, paramData.id_ );
            writeParamData( param, paramData, defaultInfo );
        
		    moduleElement->LinkEndChild( param );
        }
	}
}


void Bank::writeLinks( Element* moduleElement, ModuleData* data )
{
	LinkDataList* links = &data->links_;
	for( LinkDataList::iterator itLinks = links->begin(); itLinks != links->end(); itLinks++ ) 
	{
		LinkData& linkData = *itLinks;
        Element* link = new Element( "Link" );
		link->SetAttribute( "source", linkData.sourceId_ );
		link->SetAttribute( "out",	  linkData.outputId_ );
		link->SetAttribute( "in",     linkData.inputId_ );

        CtrlInfo defaultInfo;
        writeParamData( link, linkData, &defaultInfo );

		moduleElement->LinkEndChild( link );
	}
}


void Bank::getXml( ModuleData* data, ostringstream& result )
{
    Document doc;
    Element* element = writeModule( data );
    doc.LinkEndChild( element );

    result << doc;
}


void Bank::getXml( Program* program, ostringstream& result )
{
    if( program )
    {
        Document doc;
        Element* element = writeProgram( program );
        doc.LinkEndChild( element );

        result << doc;
    }
}


void Bank::createFromXml( istringstream& is, Program& program )
{
    Document doc;
    try
    {
	    doc.Parse( is.str(), true );
        Node* root = &doc;

        Node* programNode = doc.FirstChild( "Program", false );
        if( programNode != NULL ) {
            root = programNode;
        }

	    Iterator< Element > it( "Module" );
	    for( it = it.begin( root ); it != it.end(); it++ )
	    {
		    ModuleData* data = new ModuleData();
		    Element* moduleElement = it.Get();
		    readModule( moduleElement, data );

		    program.addModule( data );
	    }
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
}


void Bank::createFromXml( istringstream& is, ProgramList& list )
{
    Document doc;
    try
    {
	    doc.Parse( is.str(), true );

        Node* moduleNode = doc.FirstChild( "Module", false );
        if( moduleNode != NULL )
        {
            Program* program = new Program();
            createFromXml( is, *program );
            list.push_back( program );
        }
        else {

	        Iterator< Element > it( "Program" );
	        for( it = it.begin( &doc ); it != it.end(); it++ )
	        {
		        Program* program = new Program();
		        Element* programElement = it.Get();
		        readProgram( programElement, program );

		        list.push_back( program );
	        }
        }
    }
    catch( const exception& e ) {
        TRACE( e.what() );
    }
}


void Bank::makeBackup()
{
    string backupPath = path_ + ".bak";

    ifstream ifs( path_.c_str(), ios::binary);
    ofstream ofs( backupPath.c_str(), ios::binary);

    ofs << ifs.rdbuf();
}

