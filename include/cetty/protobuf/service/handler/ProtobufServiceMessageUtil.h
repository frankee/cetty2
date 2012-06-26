#ifndef __PROTOBUF_UTIL__
#define __PROTOBUF_UTIL__

#include <cetty/buffer/ChannelBufferFwd.h>
namespace cetty{
namespace protobuf{
namespace service{
namespace handler{

	enum MyWireType {
		WIRETYPE_VARINT           = 0,
		WIRETYPE_FIXED64          = 1,
		WIRETYPE_LENGTH_DELIMITED = 2,
		WIRETYPE_START_GROUP      = 3,
		WIRETYPE_END_GROUP        = 4,
		WIRETYPE_FIXED32          = 5,
	};

	class ProtobufUtil
	{
	public:
		ProtobufUtil(){}
		~ProtobufUtil(){}

		inline static MyWireType GetTagWireType(uint64_t& tag) {
			return static_cast<MyWireType>(tag & kTagTypeMask);
		}

		inline static int GetTagFieldNumber(uint64_t& tag) {
			return static_cast<int>(tag >> kTagTypeBits);
		}

		static  void*	   fieldDecode(ChannelBufferPtr& buffer, MyWireType wireType,int fieldNum);
		static  uint64_t   fixed64Decode(ChannelBufferPtr& buffer);
		static  uint64_t   fixed32Decode(ChannelBufferPtr& buffer);
		static  uint64_t   varintDecode(ChannelBufferPtr& buffer);
		static  void*	   lenthDelimitedDecode(ChannelBufferPtr& buffer,int fieldNum);


		static  void	   fieldEncode(ChannelBufferPtr& buffer);
		static  uint64_t   fixed64Encode(ChannelBufferPtr& buffer,uint64_t data);
		static  uint64_t   fixed32Encode(ChannelBufferPtr& buffer,int data);
		static  uint64_t   varintEncode(ChannelBufferPtr& buffer,uint64_t val,bool  flag);
		static  void	   lenthDelimitedEncode(ChannelBufferPtr& buffer,void* data,int fieldNum);

		static  void	   writeTag(ChannelBufferPtr& buffer,uint64_t fieldNum,uint64_t type);
		static  void	   writeLen(ChannelBufferPtr& buffer,uint64_t len);
	public:
		static const uint64_t kTagTypeBits = 3;
		// Mask for those bits.
		static const uint64_t kTagTypeMask = (1 << kTagTypeBits) - 1;
	};

}
}
}
}
#endif