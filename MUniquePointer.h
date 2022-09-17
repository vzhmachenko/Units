
#ifndef MUNIQUEPOINTER_H
#define MUNIQUEPOINTER_H
#include <memory>
#include <utility>

namespace vz {
	template <typename T, typename Deleter = std::default_delete<T>>
	class MUnique_ptr {
	protected:

		T* dataPointer = nullptr;
		Deleter deleter;

	public:

		MUnique_ptr (T* ptr = nullptr)
			: dataPointer (ptr)
		{

		}


		MUnique_ptr (MUnique_ptr const&) = delete;


		MUnique_ptr (MUnique_ptr&& u) {
			dataPointer = u.dataPointer;
			u.dataPointer = nullptr;
		}


		MUnique_ptr& operator= (MUnique_ptr const&) = delete;


		MUnique_ptr& operator= (MUnique_ptr&& u) {
			if (dataPointer == u.dataPointer) return *this;
			//delete dataPointer;
			deleter(dataPointer);

			dataPointer = u.dataPointer;
			u.dataPointer = nullptr;
			return *this;
		}

		~MUnique_ptr() {
			//delete dataPointer;
			deleter(dataPointer);
		}

	// Modifiers

		T* release() noexcept {
			T* ret = dataPointer;
			dataPointer = nullptr;
			return ret;
		}


		void reset(T* ptr = nullptr) noexcept {
			//delete dataPointer;
			deleter(dataPointer);
			dataPointer = ptr;
		}

		void swap(T& other) {
			T tmp = std::move(*this);
			*this = std::move(other);
			other = std::move(tmp);
		}

	// Observers

		T* get() const noexcept {
			return dataPointer;
		}


		operator bool() const noexcept {
			return dataPointer;
		}

	// Single-object version, unique_ptr<T>

		T& operator*() const {
			return *dataPointer;
		}

		T* operator->() const {
			return dataPointer;
		}
	};


/* ====================================================================== */
/* ====================================================================== */
/* ====================================================================== */

	template <typename T, class Deleter>
	class  MUnique_ptr<T[], Deleter> {
	protected:

		T* dataPointer = nullptr;
		Deleter deleter;

	public:

		MUnique_ptr (T* ptr = nullptr)
			: dataPointer (ptr)
		{

		}


		MUnique_ptr (MUnique_ptr const&) = delete;


		MUnique_ptr (MUnique_ptr&& u) {
			dataPointer = u.dataPointer;
			u.dataPointer = nullptr;
		}


		MUnique_ptr& operator= (MUnique_ptr const&) = delete;


		MUnique_ptr& operator= (MUnique_ptr&& u) {
			if (dataPointer == u.dataPointer) return *this;
			//delete [] dataPointer;
			deleter(dataPointer);

			dataPointer = u.dataPointer;
			u.dataPointer = nullptr;
			return *this;
		}

		~MUnique_ptr() {
			//delete [] dataPointer;
			deleter(dataPointer);
		}

	// Modifiers

		T* release() noexcept {
			T* ret = dataPointer;
			dataPointer = nullptr;
			return ret;
		}


		void reset(T* ptr = nullptr) noexcept {
			//delete [] dataPointer;
			deleter(dataPointer);
			dataPointer = ptr;
		}

		void swap(T& other) {
			T tmp = std::move(*this);
			*this = std::move(other);
			other = std::move(tmp);
		}

	// Observers

		T* get() const noexcept {
			return dataPointer;
		}


		operator bool() const noexcept {
			return dataPointer;
		}

		// Array version, unique_ptr<T[]>
		T& operator[] (size_t i) const {
			return *(dataPointer + i);
		}

	};

}


#endif // MUNIQUEPOINTER_H
