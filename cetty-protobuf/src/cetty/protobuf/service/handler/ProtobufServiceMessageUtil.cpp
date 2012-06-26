#include <cetty/protobuf/service/handler/ProtobufServiceMessageUtil.h>
#include <cetty/channel/ChannelMessage.h>
#include <cetty/protobuf/service/proto/service.pb.h>

using namespace cetty::protobuf::service::handler;
using namespace cetty::buffer;

//parse the field  dynamic
void*  ProtobufUtil::fieldDecode(ChannelBufferPtr& buffer, MyWireType wireType,int fieldNum)
{
	void* ret = NULL;
	switch(wireType)
	{
		case WIRETYPE_VARINT:
			ret = varintDecode(buffer);
			break;
		case WIRETYPE_FIXED64:
			ret = fixed64Decode(buffer);
			break;
		case WIRETYPE_LENGTH_DELIMITED:
			ret = lenthDelimitedDecode(buffer,fieldNum);
			break;
		case WIRETYPE_FIXED32:
			ret = fixed32Decode(buffer);
			break;
		default:
			break;
	}
	return ret;
}

uint64_t ProtobufUtil::fixed64Decode(ChannelBufferPtr& buffer) {
	uint64_t id = buffer->readLong();
	return id;
}


uint64_t ProtobufUtil::fixed32Decode(ChannelBufferPtr& buffer) {
	uint32_t id = buffer->readUnsignedInt();
	return id;
}

uint64_t ProtobufUtil::varintDecode(ChannelBufferPtr& buffer) {
    uint8_t temp;
    uint64_t ret;
    //boost::int8_t ret[msgTypeSize];
	int i = 0;
    while(true)
	{
        temp = buffer->readByte();
        //如果有后续的字节需要拼接
        if (temp&0x80) {
            //先左右移一下去掉mab，然后再右移指定的位数，因为varint是低位在前，高位在后
            ret |= (temp<<1>>1)<<(7*i);
			i++;
        }
        else {
            ret |= temp<<(7*i);
            //达到最后一个数据，并且是最高位
            break;
        }
    }
    return ret;
}

void* ProtobufUtil::lenthDelimitedDecode(ChannelBufferPtr& buffer,int fieldNum) {
	uint64_t msgSize;
	void* ret = NULL;
	uint64_t tag;

	switch(fieldNum)
	{
		//for string
		case 3:
		case 4:
		case 6:
		case 7:
			msgSize =  varintDecode(buffer);
			std::string servStr;
			buffer->readBytes(&servStr,msgSize);
			ret = &servStr;
			break;
		//for msgType
		case 1:
			msgSize =  varintDecode(buffer);
			tag = varintDecode(buffer);
			ret = varintDecode(buffer);
			break;
		//for errorcode
		case 5:
			msgSize =  varintDecode(buffer);
			tag = varintDecode(buffer);
			ret = varintDecode(buffer);
			break;
		default:
			break;
	}
	return ret;
}




void ProtobufUtil::fieldEncode(ChannelBufferPtr& buffer)
{
	
}
uint64_t  ProtobufUtil::fixed64Encode(ChannelBufferPtr& buffer,uint64_t data)
{
	buffer->writeInt(data);
}
uint64_t  ProtobufUtil::fixed32Encode(ChannelBufferPtr& buffer,int data)
{
	buffer->writeInt(data);
}
//the flag is indict that if varint is embedded，1:embedded  0:unembedded
//只有嵌套的varint里面才需要写 tag
uint64_t  ProtobufUtil::varintEncode(ChannelBufferPtr& buffer,uint64_t val,bool  flag)
{
	uint8_t* buf;
	uint64_t varintSize = 0;

	do {
		uint8_t byte = val & 0x7f;
		val >>= 7;
		if(val) byte |= 0x80;
		*buf++ = byte;
		varintSize++;
	} while(val);

	
	//如果是嵌套的，先把字节数写在前面，字节数也是varint
	if(flag)
	{
		varintEncode(buffer,varintSize,0);
		//里面嵌套的头
		writeTag(buffer,1,0);
	}
	Array array((char*)(buf-varintSize),varintSize);
	buffer->writeBytes(array);
}


void ProtobufUtil::lenthDelimitedEncode(ChannelBufferPtr& buffer,void* data,int fieldNum)
{
	switch(fieldNum)
	{
		//write for string
		case 3:
			writeTag(buffer,3,2);
			writeLen(buffer,(static_cast<std::string*>(data))->size());
			buffer->writeBytes(static_cast<std::string*>(data)->size());
			break;
		case 4:
			writeTag(buffer,4,2);
			writeLen(buffer,((std::string)data).size());
			buffer->writeBytes((std::string)data);
			break;
		case 1:
			writeTag(buffer,1,2);
			//flag=1 表明这是嵌套的，在写这个值前会先写这个值的字节数
			varintEncode(buffer,(uint64_t)data,1);
			break;
		case 5:
			writeTag(buffer,5,2);
			//flag=1 表明这是嵌套的，在写这个值前会先写这个值的字节数
			varintEncode(buffer,(uint64_t)data,1);
			break;
		default:
			break;	
	}
}

void ProtobufUtil::writeTag(ChannelBufferPtr& buffer,uint64_t fieldNum,uint64_t type)
{
	uint64_t tag = (fieldNum<<3)|type;
	varintEncode(buffer,tag,0);
}

void ProtobufUtil::writeLen(ChannelBufferPtr& buffer,uint64_t len)
{
	varintEncode(buffer,len,0);
}