//
// Created by nikita on 2/7/18.
//

#ifndef VANGERS_RING_BUFFER_H
#define VANGERS_RING_BUFFER_H

namespace util{
	template <class T> class RingBuffer {
	private:
		T* buffer;
		int capacity;
		int begin;
		int size;
	public:
		explicit RingBuffer(int capacity):capacity(capacity),begin(0),size(0){
			buffer = new T[capacity];
		}

		int get_capacity() const { return capacity; }

		virtual ~RingBuffer(){
			delete[] buffer;
		}

		T at(int index) const {
			return buffer[(begin + index) % capacity];
		}

		int get_size() const {
			return size;
		}

		void add(const T& value){
			int end = (begin + size) % capacity;
			buffer[end] = value;
			if(size == capacity){
				begin = (begin + 1) % capacity;
			} else {
				size += 1;
			}
		}

		void clear(){
			begin = 0;
			size = 0;
		}
	};

}

#endif //VANGERS_RING_BUFFER_H
