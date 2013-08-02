
/**
 * Buffer 中获取信息后 会重置当前位置
 */
class Record{
	Record(void *cmd,unsigned int len)
	{
		conntents = new char[len];
		memcpy(contents,cmd,len)
		contentSize = len;
	}
	Record()
	{
		offset = 0;
		contentSize = 0;
	}
	~Record()
	{
		if (contents) delete contents;
		contents = NULL;
	}
	unsigned int copy(void *buffer,unsigned int len)
	{
		if (empty()) return 0;
		len = leftsize() < len ? leftsize(): len;
		memcpy(buffer,conents + offset,len);
		offset += len;
		return len;
	}
	unsigned int leftsize()
	{
		return contentSize - offset;
	}
	unsigned int offset;
	bool empty()
	{
		return offset == contentSize;
	}
	unsigned int contentSize;
	unsigned char *conntents;
};