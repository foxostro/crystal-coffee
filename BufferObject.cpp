#include "glheaders.h"
#include "vec/vec.h"
#include "BufferObject.h"

template<typename ELEMENT>
BufferObject<ELEMENT>::~BufferObject() {
	glDeleteBuffers(1, &handle);
	delete [] buffer;
}

template<typename ELEMENT>
BufferObject<ELEMENT>::BufferObject()
: locked(false),
  numElements(0),
  buffer(0),
  handle(0),
  usage(STREAM_DRAW) {
	// Do Nothing
}

template<typename ELEMENT>
BufferObject<ELEMENT>::BufferObject(int numElements,
                                        const ELEMENT *buffer)
: locked(false),
  numElements(0),
  buffer(0),
  handle(0),
  usage(STREAM_DRAW) {
	recreate(numElements, buffer, STREAM_DRAW);
}

template<typename ELEMENT>
BufferObject<ELEMENT>::BufferObject(const BufferObject &copyMe)
: locked(false),
  numElements(0),
  buffer(0),
  handle(0),
  usage(STREAM_DRAW) {
	recreate(copyMe.numElements, copyMe.buffer, copyMe.usage);
}

template<typename ELEMENT>
BufferObject<ELEMENT> * BufferObject<ELEMENT>::clone(void) const {
	return new BufferObject<ELEMENT>(*this);
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::recreate(int numElements,
                                     const ELEMENT *buffer,
                                     BUFFER_USAGE usage) {
	assert(!locked && "Cannot realloc a locked buffer!");
	assert(numElements>=0 && "Parameter \'numElements\' < 0");
	
	this->usage = usage;
	
	create_cpu_buffer(numElements, buffer);
	create_gpu_buffer(numElements, buffer, getGLUsageToken(usage));
}

template<typename ELEMENT>
int BufferObject<ELEMENT>::getNumber() const {
	return numElements;
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::bind() const {
	assert(!locked && "Cannot bind buffer for use when the buffer is locked!");
	glBindBuffer(getTarget(), handle);
}

template<typename ELEMENT>
void* BufferObject<ELEMENT>::lock() {
	void *mapped_buffer=0;
	
	assert(!locked && "Cannot lock a buffer that is already locked!");
	locked=true;
	
	glBindBuffer(getTarget(), handle);
	mapped_buffer = glMapBuffer(getTarget(), GL_READ_WRITE);
	
	return mapped_buffer;
}

template<typename ELEMENT>
void* BufferObject<ELEMENT>::read_lock() {
	void *mapped_buffer=0;
	
	assert(!locked && "Cannot lock a buffer that is already locked!");
	locked=true;
	
	glBindBuffer(getTarget(), handle);
	mapped_buffer = glMapBuffer(getTarget(), GL_READ_ONLY);
	
	return mapped_buffer;
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::unlock() const {
	assert(locked && "Cannot unlock a buffer that is not locked!");
	locked=false;
	
	glBindBuffer(getTarget(), handle);
	glUnmapBuffer(getTarget());
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::create_cpu_buffer( int numElements, const ELEMENT * buffer ) {
	delete [] (this->buffer);
	this->buffer = 0;
	this->numElements = numElements;
	
	if (numElements>0) {
		this->buffer = new ELEMENT[numElements];
		this->numElements = numElements;
		
		if (buffer != 0) {
			memcpy(this->buffer, buffer, sizeof(ELEMENT)*numElements);
		} else {
			memset(this->buffer, 0, sizeof(ELEMENT)*numElements);
		}
	}
}

template<typename ELEMENT>
void BufferObject<ELEMENT>::create_gpu_buffer(int numElements,
  const ELEMENT * buffer,
  GLenum usage) {
	GLenum target = getTarget();
		
	// Create and fill a buffer object on the GPU
	glGenBuffers(1, &handle);
	glBindBuffer(target, handle);
	glBufferData(target,
	             sizeof(ELEMENT) * numElements,
	             buffer,
	             usage);
}

template<typename ELEMENT>
GLenum BufferObject<ELEMENT>::getTarget() {
	return GL_ARRAY_BUFFER;
}

/**
Element array buffers have a different target
specification from vertex array buffers.
*/
template<> GLenum BufferObject<index_t>::getTarget() {
	return GL_ELEMENT_ARRAY_BUFFER;
}

template<typename ELEMENT>
GLenum BufferObject<ELEMENT>::getGLUsageToken(BUFFER_USAGE usage) {
	switch (usage) {
	case STREAM_DRAW:
		return GL_STREAM_DRAW;
	case STREAM_READ:
		return GL_STREAM_READ;
	case STREAM_COPY:
		return GL_STREAM_COPY;
	case STATIC_DRAW:
		return GL_STATIC_DRAW;
	case STATIC_READ:
		return GL_STATIC_READ;
	case STATIC_COPY:
		return GL_STATIC_COPY;
	case DYNAMIC_DRAW:
		return GL_DYNAMIC_DRAW;
	case DYNAMIC_READ:
		return GL_DYNAMIC_READ;
	case DYNAMIC_COPY:
		return GL_DYNAMIC_COPY;
	default:
		assert(!"Invalid enumerant");
		return 0;
	}
}

// template class instantiations
// (see http://www.codeproject.com/cpp/templatesourceorg.asp)
template class BufferObject<Vec4>;
template class BufferObject<Vec3>;
template class BufferObject<Vec2>;
template class BufferObject<index_t>;
