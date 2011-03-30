/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public License
 **************************************************************/
//Headers
#include "handler_Report.hpp"

structRow::structRow(unsigned &col){///Creates defined number of columns
    while(data.size() < col)
        data.push_back("");
}
///Constractor set all args with default values
handler_Report::handler_Report():\
                            strSpacer(""),
                            p_vecstr_Log(NULL),\
                            p_vecstr_formattedTXT(NULL),\
                            pathReport(""),
                            strReportFile("Report"),
                            bLog(false),\
                            bFormattedTxt(true),\
                            bGenXML(false),\
                            uiMultiplySpacer(1),\
                            uiMaxCols(1),\
                            strSpacerChar(" "){
}

handler_Report::~handler_Report(){
    cerr<<"Destructor: handler_Report"<<endl;
    if ( p_vecstr_Log ){
        delete p_vecstr_Log;
        p_vecstr_Log = NULL;
    }
    if ( p_vecstr_formattedTXT ){

        delete p_vecstr_formattedTXT;
        p_vecstr_formattedTXT = NULL;
    }
}
///Set Args determinates functionality
void handler_Report::setArgs(){
    ++uiMaxCols;
    if ( bLog ){
        if (!p_vecstr_formattedTXT){
            p_vecstr_Log = new vector<string>;
            while( p_vecstr_Log->size() < uiMaxCols)
                p_vecstr_Log->push_back("");
        }else
            cerr<<"setArgs runned more then once fix code"<<endl;
    }
    if ( bFormattedTxt ){
        if ( !p_vecstr_formattedTXT)
            p_vecstr_formattedTXT = new vector<structRow>;
        else
            cerr<<"setArgs runned more then once fix code"<<endl;
    }
    for(unsigned i=0; i<uiMaxCols; i++)
        strSpacer += strSpacerChar;
}
///Will add data to specified buffers
void handler_Report::addStatData(string &desc, string &val,const unsigned &row,const unsigned &col){
    try{
    if (bLog){
        p_vecstr_Log->at(col).append(desc+" "+val+"\n");
    }
    if(bFormattedTxt){
        if ( p_vecstr_formattedTXT->size() < row + 1 )
            p_vecstr_formattedTXT->push_back(uiMaxCols);
        if ( p_vecstr_formattedTXT->at(row).data.at(0) == "")
            p_vecstr_formattedTXT->at(row).data.at(0) = desc;
        if ( p_vecstr_formattedTXT->at(row).data.at(0) == desc )
            p_vecstr_formattedTXT->at(row).data.at(col) = val;
        else{
            cerr<<"Error dected at position ("<<col<<","<<row<<")\n"<<
            "desc["<<desc<<"]val["<<val<<"]\nMaxCols:"<<uiMaxCols<<
            "\n2D size("<<p_vecstr_formattedTXT->size()<<","<<p_vecstr_formattedTXT->at(0).data.size()<<")"<<endl;
            cerr<<"Data at error position["<<p_vecstr_formattedTXT->at(row).data.at(0)<<"][" <<p_vecstr_formattedTXT->at(row).data.at(col)<<"]"<<endl;
        }
    }
    }catch(std::exception &e){
        cerr<<"Faile to add new statistic data("<<col<<
        ","<<row<<")\n"<<"desc["<<desc<<"]\nval["<<val<<"]MaxCols:"<<uiMaxCols<<
        "\n2D size("<<p_vecstr_formattedTXT->size()<<","<<p_vecstr_formattedTXT->at(0).data.size()<<")"<<endl;
        if(p_vecstr_formattedTXT->size() <= row ){
            cerr<<"Vector Rows gone out of range"<<endl;
        }
        if(p_vecstr_formattedTXT->at(0).data.size() <= col){
            cerr<<"Vector Columns gone out of range"<<endl;
        }
        cerr<<e.what()<<endl;
        raise(SIGABRT);
    }
}

uint8_t handler_Report::findAndAdd (string &desc,string &val,unsigned &row,unsigned &col){
    /*if (!desc or !val){
        cerr<<"handler_Report recived NULL data"<<endl;
        return 2;
    }*/
    if ( !p_vecstr_formattedTXT){
        cerr<<"2D data vector has not been set! Run setArgs() first"<<endl;
        return 3;
    }
    if ( p_vecstr_formattedTXT->size() < 1){
        addStatData(desc,val,row,col);
        return 0;
    }
    unsigned i=0;
    for (vector<structRow>::iterator it = p_vecstr_formattedTXT->begin();
        it != p_vecstr_formattedTXT->end();
            it++){
                if ( it->data.at(0) == desc ){
                    string strtmp;
                    if (it->data.at(col).length() > 0)
                        strtmp = string(it->data.at(col))+"|"+val;
                    else
                        strtmp = val;
                    addStatData(it->data.at(0),
                                strtmp,
                                i,col);
                    return 1;
                }
                i++;
    }
    i > row ? row = i: row;
    addStatData(desc,val,row,col);
    return 0;
}

void handler_Report::FormatDataInVector(){
    if ( !p_vecstr_formattedTXT )
        return;
    vector<unsigned> vec_uiLenght;
    while ( p_vecstr_formattedTXT->begin()->data.size() > vec_uiLenght.size())
        vec_uiLenght.push_back(0);
    vector<unsigned>::iterator itLe = vec_uiLenght.begin();

    for( vector<structRow>::iterator itRows = p_vecstr_formattedTXT->begin();
        itRows != p_vecstr_formattedTXT->end();
        itRows++){
            for( vector<string>::iterator itCols = itRows->data.begin();
                itCols != itRows->data.end() and itLe != vec_uiLenght.end();
                itCols++, itLe++){
                    if (itCols->length() > *itLe)
                        *itLe = itCols->length();
                }
            itLe = vec_uiLenght.begin();

        }
    itLe = vec_uiLenght.begin();
    for( vector<structRow>::iterator itRows = p_vecstr_formattedTXT->begin();
        itRows != p_vecstr_formattedTXT->end();
        itRows++){
            for( vector<string>::iterator itCols = itRows->data.begin();
                itCols != itRows->data.end() and itLe != vec_uiLenght.end();
                itCols++ , itLe++){
                    while (itCols->length() < *itLe)
                        itCols->append(" ");
                }
            itLe = vec_uiLenght.begin();
        }
}

///Return refernce to string value generated from vector
string handler_Report::GeneratDataFromVector(){
    string str ="";
    if ( p_vecstr_formattedTXT ){
        for( vector<structRow>::iterator itRows = p_vecstr_formattedTXT->begin();
            itRows != p_vecstr_formattedTXT->end();
            itRows++){
                for( vector<string>::iterator itCols = itRows->data.begin();
                    itCols != itRows->data.end();
                    itCols++){
                        str.append(*itCols+strSpacer);
                    }
                    str.erase(str.rfind(strSpacer));
                    str.append("\n");
            }
    }
    return str;
}

string handler_Report::GenerateXMLFromVector(){
    string str ="Not Implemented yet";
    return str;
}

void handler_Report::SaveToDisk(boost::filesystem::path path){
    if ( bFormattedTxt ){
        path.replace_extension(".txt");
        FormatDataInVector();
        blIO::SimpleWriteToFile(path, GeneratDataFromVector() );
        blIO::SimpleWriteToFile(path, string("\n") );
    }
    if (bLog){
        path.replace_extension(".log");
        for(unsigned i =0; i< uiMaxCols; i++){
            blIO::SimpleWriteToFile(path, p_vecstr_Log->at(i) );
            blIO::SimpleWriteToFile(path, string("\n") );
        }
    }
    if (bGenXML){
        path.replace_extension(".xml");
        blIO::SimpleWriteToFile(path, GenerateXMLFromVector() );
    }
}
