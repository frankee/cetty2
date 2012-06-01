#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFJSONFORMATTER_H)
#define CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFJSONFORMATTER_H

#if !defined(_MSC_VER)
#include <stdint.h>
#endif

// Local Variables:
// mode: c++
// End:


#include <cetty/handler/rpc/protobuf/ProtobufFormatter.h>

namespace google {
namespace protobuf {
class FieldDescriptor;
}
}

namespace cetty {
namespace handler {
namespace rpc {
namespace protobuf {

class ProtobufJsonFormatter : public ProtobufFormatter {
private:
    class JsonGenerator;

public:
    ProtobufJsonFormatter() {}
    virtual ~ProtobufJsonFormatter() {}

    virtual void serializeToString(const google::protobuf::Message& message, std::string* str);

private:
    void serializeMessage(const google::protobuf::Message& message, JsonGenerator& json);
    bool serializeField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator& json);
    void serializeSingleField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator& json);

    void printFieldRepeatedValue(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator& json);
    void printFieldValue(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, JsonGenerator json);

    /**
     * An inner class for writing text to the output stream.
     */
    class JsonGenerator {
    public:
#ifdef _MSC_VER
        typedef __int32 int32;
        typedef __int64 int64;
        typedef unsigned __int32 uint32;
        typedef unsigned __int64 uint64;
#else
        typedef int32_t int32;
        typedef int64_t int64;;
        typedef uint32_t uint32;
        typedef uint64_t uint64;
#endif

        JsonGenerator(std::string& output) : output(output), styled(false) {
        }

        ~JsonGenerator() {}

        /**
         * Indent text by two spaces. After calling Indent(), two spaces will be inserted at the
         * beginning of each line of text. Indent() may be called multiple times to produce deeper
         * indents.
         */
        void indent();

        /**
         * Reduces the current indent level by two spaces, or crashes if the indent level is zero.
         */
        void outdent();

        /**
         * Print text to the output stream.
         */
        void append(const char* text);
        void append(const std::string& text);

        void append(int32 value);
        void append(uint32 value);
        void append(int64 value);
        void append(uint64 value);

        void append(double value);
        void append(bool value);

        void append(const char* data, int size);

    private:
        bool styled;
        std::string& output;
    };
};

}
}
}
}

#endif //#if !defined(CETTY_HANDLER_RPC_PROTOBUF_PROTOBUFJSONFORMATTER_H)

// Local Variables:
// mode: c++
// End:

