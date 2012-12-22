// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#ifndef GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_H__
#define GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_H__

#include <string>
#include <google/protobuf/stubs/common.h>

namespace google {
namespace protobuf {

class Descriptor;
class FieldDescriptor;

namespace io {
class Printer;             // printer.h
}

}
}

namespace cetty {
namespace config {
namespace generator {
namespace cpp {

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace std;

class MessageGenerator {
public:
    // See generator.cc for the meaning of dllexport_decl.
    explicit MessageGenerator(const Descriptor* descriptor,
                              const string& dllexport_decl);
    ~MessageGenerator();

    // Header stuff.

    // Generate foward declarations for this class and all its nested types.
    void GenerateForwardDeclaration(io::Printer* printer);

    // Generate definitions for this class and all its nested types.
    void GenerateClassDefinition(io::Printer* printer);

    // Source file stuff.
    // Generate constructors and destructor.
    void GenerateStructors(io::Printer* printer);

    void GenerateTypeRegistrations(io::Printer* printer);

    void GenerateCommandLines(io::Printer* printer);

private:
    void Indent(io::Printer* printer);

    void Outdent(io::Printer* printer);

    bool isNestType(const Descriptor* descriptor, const FieldDescriptor* fieldDescriptor);

private:
    const Descriptor* descriptor_;
    string classname_;
    string dllexport_decl_;
    scoped_array<scoped_ptr<MessageGenerator> > nested_generators_;

    GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(MessageGenerator);
};

}
}
}
}

#endif  // GOOGLE_PROTOBUF_COMPILER_CPP_MESSAGE_H__
