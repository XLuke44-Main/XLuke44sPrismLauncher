#pragma once

#include <QString>

namespace MessageLevel {
enum Enum {
    Unknown,   
    StdOut,    
    StdErr,    
    Launcher,  
    Debug,     
    Info,      
    Message,   
    Warning,   
    Error,     
    Fatal,     
};
MessageLevel::Enum getLevel(const QString& levelName);

MessageLevel::Enum fromLine(QString& line);
}   
