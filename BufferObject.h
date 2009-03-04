#ifndef _BUFFER_RESOURCE_H_
#define _BUFFER_RESOURCE_H_

#define MESH_INDEX_FORMAT_IS_UINT 1

#if MESH_INDEX_FORMAT_IS_UINT
#define MESH_INDEX_FORMAT (GLenum)(GL_UNSIGNED_INT)
typedef unsigned int index_t;
#else
#define MESH_INDEX_FORMAT (GLenum)(GL_UNSIGNED_SHORT)
typedef unsigned short index_t;
#endif

enum BUFFER_USAGE {
	STREAM_DRAW,
	STREAM_READ,
	STREAM_COPY,
	STATIC_DRAW,
	STATIC_READ,
	STATIC_COPY,
	DYNAMIC_DRAW,
	DYNAMIC_READ,
	DYNAMIC_COPY
};


/**
Contains a buffer of graphically related data such as an index array or a
vertex array. This data may be stored in memory on the graphics device after
being submitted.
*/
template<typename ELEMENT>
class BufferObject {
public:
	/** Destructor */
	~BufferObject();
	
	/** Default Constructor */
	BufferObject();
	
	/**
	Constructor
	@param numElements Number of elements in the buffer
	@param buffer Readable on the client-side.  Buffer is copied here and
	no ownership is passed. If null is passed for the 'buffer' parameter
	then a buffer is allocated internally (accessible through lock) but
	it will be uninitialized upon construction.
	*/
	BufferObject(int numElements, const ELEMENT *buffer);
	
	/**
	Copy constructor. Copies the contents of a given buffer.
	@param copyMe Buffer to clone
	*/
	BufferObject(const BufferObject &copyMe);
	
	/**
	Creates a copy of this buffer.
	The copy is created on the heap with new, and it is the responsibility of
	the caller to free it.
	@return New object with copy of this buffer
	*/
	BufferObject<ELEMENT> * clone(void) const;

	void create(int numElements, const ELEMENT *buffer, BUFFER_USAGE usage)
	{
		recreate(numElements, buffer, usage);
	}
	
	/**
	Reallocates memory for the buffer
	@param numElements Number of elements in the buffer.  Must be greater
	than zero unless both numElements=0 and buffer=0
	@param buffer Readable on the client-side.  Buffer is copied here and
	no ownership is passed. If null is passed for the 'buffer' parameter
	then a buffer is allocated internally (accessible through lock) but
	it will be uninitialized upon construction.
	@param usage Enumerant describing how the buffer will be used.  This
	is a hint to the graphics driver as to how the buffer should be stored
	in memory.
	*/
	void recreate(int numElements, const ELEMENT *buffer, BUFFER_USAGE usage);
	
	/**
	Gets the number of elements in the buffer
	@return Number of elements in the buffer
	*/
	int getNumber() const;
	
	/** Binds the buffer for use on the GPU */
	void bind() const;
	
	/**
	Locks the buffer to allow read-write access by the client.
	@return elements array
	*/
	void* lock();
	
	/**
	Obtaind read access to the buffer. Do not rely on write access.
	@return elements array
	*/
	void* read_lock();
	
	/**
	Unlocks the buffer and removes memory maps.
	Only call on locked buffers.
	*/
	void unlock() const;
	
private:
	void create_cpu_buffer(int numElements, const ELEMENT * buffer);
	
	void create_gpu_buffer(int numElements,
	                       const ELEMENT * buffer,
	                       GLenum usage);
	                       
	static GLenum getTarget();
	
	static GLenum getGLUsageToken(BUFFER_USAGE usage);
	
private:
	/** Indicates that the buffer is currently locked */
	mutable bool locked;
	
	/** Number of elements in the buffer */
	int numElements;
	
	/** Buffer, stored on the client-side */
	ELEMENT *buffer;
	
	/** OpenGL buffer object name */
	GLuint handle;
	
	/** Store this so if we are cloned, the copy can set usage properly */
	BUFFER_USAGE usage;
};

#endif
