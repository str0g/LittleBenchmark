/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers
#include "stats_keeper.hpp"

vector_str::vector_str(unsigned &row){
    while(data.size() < row)
        data.push_back("");
}
///Constractor set all args with default values
stats_keeper::stats_keeper():\
                            p_vecstr_Log(NULL),\
                            p_vecstr_formattedTXT(NULL),\
                            bLog(false),\
                            bFormattedTxt(true),\
                            bGenXML(false),\
                            uiMultiplySpacer(1),\
                            uiMaxCols(1),\
                            strSpacerChar(" "),\
                            strSpacer(""){
}

stats_keeper::~stats_keeper(){
    cerr<<"Disctructing: stats_keeper"<<endl;
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
void stats_keeper::setArgs(){
    ++uiMaxCols;
    if ( bLog )
        if (!p_vecstr_formattedTXT){
            p_vecstr_Log = new vector<string>;
            while( p_vecstr_Log->size() < uiMaxCols)
                p_vecstr_Log->push_back("");
        }else
            cerr<<"setArgs runned more then once fix code"<<endl;
    if ( bFormattedTxt )
        if ( !p_vecstr_formattedTXT)
            p_vecstr_formattedTXT = new vector<vector_str>;
        else
            cerr<<"setArgs runned more then once fix code"<<endl;
    for(int i=0; i<uiMaxCols; i++)
        strSpacer += strSpacerChar;
}
///Will add data to specified buffers
void stats_keeper::addStatData(string *desc, string *val, unsigned col, unsigned row){
    try{
    col = col -1;
    if (bLog){
        p_vecstr_Log->at(row).append(*desc+" "+*val+"\n");
    }
    if(bFormattedTxt){
        if ( p_vecstr_formattedTXT->size() < col +1 )
            p_vecstr_formattedTXT->push_back(uiMaxCols);
        if ( p_vecstr_formattedTXT->at(col).data.at(0) == "")
            p_vecstr_formattedTXT->at(col).data.at(0) = *desc;
        if ( p_vecstr_formattedTXT->at(col).data.at(0) == *desc )
            p_vecstr_formattedTXT->at(col).data.at(row) = *val;
        else
            cerr<<"Error dected at position ("<<row<<","<<col+1<<")\n"<<
            "desc["<<*desc<<"]\nval["<<*val<<"]MaxCols:"<<uiMaxCols<<
            "\n2D size("<<p_vecstr_formattedTXT->size()<<","<<p_vecstr_formattedTXT->at(0).data.size()<<")"<<endl;
    }
    }catch(std::exception &e){
        cerr<<"Faile to add new statistic data("<<row<<
        ","<<col+1<<")\n"<<"desc["<<*desc<<"]\nval["<<*val<<"]MaxCols:"<<uiMaxCols<<
        "\n2D size("<<p_vecstr_formattedTXT->size()<<","<<p_vecstr_formattedTXT->at(0).data.size()<<")"<<endl;
        if(p_vecstr_formattedTXT->size() <= col ){
            cerr<<"Vector Column gone out of range"<<endl;
        }
        if(p_vecstr_formattedTXT->at(0).data.size() <= row){
            cerr<<"Vector Rows gone out of range"<<endl;
        }
        cerr<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
}

void stats_keeper::FormatDataInVector(){
    if ( !p_vecstr_formattedTXT )
        return;
    vector<unsigned> vec_uiLenght;
    while ( p_vecstr_formattedTXT->begin()->data.size() > vec_uiLenght.size())
        vec_uiLenght.push_back(0);
    vector<unsigned>::iterator itLe = vec_uiLenght.begin();

    for( vector<vector_str>::iterator itRows = p_vecstr_formattedTXT->begin();
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
    for( vector<vector_str>::iterator itRows = p_vecstr_formattedTXT->begin();
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
string stats_keeper::GeneratDataFromVector(){
    string str ="";
    if ( p_vecstr_formattedTXT ){
        for( vector<vector_str>::iterator itRows = p_vecstr_formattedTXT->begin();
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

string stats_keeper::GenerateXMLFromVector(){
}

void stats_keeper::SaveToDisk(boost::filesystem::path path){
    if ( bFormattedTxt ){
        path.replace_extension(".txt");
        FormatDataInVector();
        myIO::SimpleWriteToFile(path, GeneratDataFromVector() );
        myIO::SimpleWriteToFile(path, string("\n") );
    }
    if (bLog){
        path.replace_extension(".log");
        for(unsigned i =0; i< uiMaxCols; i++){
            myIO::SimpleWriteToFile(path, p_vecstr_Log->at(i) );
            myIO::SimpleWriteToFile(path, string("\n") );
        }
    }
    if (bGenXML){
        path.replace_extension(".xml");
        myIO::SimpleWriteToFile(path, GenerateXMLFromVector() );
    }
}
