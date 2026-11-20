#pragma once

#include <QString>

#include "Library.h"

class Agent;

using AgentPtr = std::shared_ptr<Agent>;

class Agent {
   public:
    Agent(LibraryPtr library, const QString& argument)
    {
        m_library = library;
        m_argument = argument;
    }

   public:  
    LibraryPtr library() { return m_library; }
    QString argument() { return m_argument; }

   protected:  

    LibraryPtr m_library;

    QString m_argument;
};
