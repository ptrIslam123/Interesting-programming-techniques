#pragma once

#include <cstdlib>
#include <cassert>

typedef unsigned int SizeType;
typedef unsigned char Byte;

template<SizeType N>
struct StaticImage;

template<bool expr>
struct EnableIf {};

template<>
struct EnableIf<true> {
    static const bool Value = true;
};


struct SharedImage {
    SharedImage():
    data_(NULL) {
        // setData<SharedImage, SharedImage, &SharedImage::data>(this); // this compile error!
    }

    Byte *data() {
        return data_;
    }

    /** ... **/
protected:
    template<class Derive, class Storager, Byte* (Storager::*getData)()>
    void setData(Storager *const self) {
        typedef StaticImage<Derive::Capacity> Base;
        assert(EnableIf<Storager::StorageCapacity == Base::Capacity>::Value);

        data_ = (self->*getData)();
    }

private:
    Byte *data_;
};

template<SizeType N>
struct StaticImage : SharedImage {
    enum {Capacity = N};
    StaticImage():
            SharedImage(),
            storager_() {
        SharedImage::setData<StaticImage<N>, StaticImage<N>::Storager<N>, &Storager<N>::storage>(&storager_);
    }

    template<SizeType K>
    struct Storager {
        enum {StorageCapacity = K};
        Storager(): storage_() {}

        Byte *storage() {
            return storage_;
        }

    private:
        Byte storage_[StorageCapacity];
    };

private:
    Storager<N> storager_;
};
