//
// Created by nikita on 26.11.2021.
//

#ifndef VANGERS_RIDSTORAGE_H
#define VANGERS_RIDSTORAGE_H

#include <unordered_map>
#include <memory>

#include "common.h"
#include "exception.h"

namespace renderer {
	template<typename TResourceId, typename TResource>
	class ResourceStorage {
	private:
		static const std::hash<int32_t> hash;
	public:
		struct ResourceIdHash {
			size_t operator()(TResourceId rid) const noexcept{
				return hash(rid.id);
			}
		};

		struct ResourceIdEquals {
			bool operator()(const TResourceId& rid1, const TResourceId& rid2) const{
				return rid1.id == rid2.id;
			}
		};


		typedef typename std::unordered_map<TResourceId, std::unique_ptr<TResource>, ResourceIdHash, ResourceIdEquals>::iterator iterator;

		ResourceStorage()
			:_next(0)
		{}

		~ResourceStorage() = default;

		TResourceId create(std::unique_ptr<TResource>&& pValue){
			TResourceId rid = TResourceId{.id=_next};
			_next += 1;

			_storage[rid] = std::move(pValue);
			return rid;
		}

		void remove(const TResourceId& rid){
			auto it = _findOrThrow(rid);
			_storage.erase(it);
		}

		std::unique_ptr<TResource>& getOrThrow(const TResourceId& rid){
			auto it = _findOrThrow(rid);
			return it->second;
		}

		iterator begin() {
			return _storage.begin();
		}

		iterator end() {
			return _storage.end();
		}

	private:
		iterator _findOrThrow(const TResourceId& rid){
			iterator it = _storage.find(rid);

			if(it == _storage.end()){
				throw ResourceDoesNotExistsException (rid);
			}
			return it;
		}

		std::unordered_map<TResourceId, std::unique_ptr<TResource>, ResourceIdHash, ResourceIdEquals> _storage;
		int32_t _next;
	};

	template <typename TResourceId, typename TResource>
	const std::hash<int32_t> ResourceStorage<TResourceId, TResource>::hash = std::hash<int32_t>();
}


#endif //VANGERS_RIDSTORAGE_H
