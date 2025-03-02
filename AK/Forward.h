/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/DefaultDelete.h>
#include <AK/SinglyLinkedListSizePolicy.h>
#include <AK/Types.h>

namespace AK {

namespace Detail {
template<size_t inline_capacity>
class ByteBuffer;
}

class Bitmap;
using ByteBuffer = Detail::ByteBuffer<32>;
class CircularBuffer;
class Error;
class GenericLexer;
class IPv4Address;
class JsonArray;
class JsonObject;
class JsonValue;
class StackInfo;
class DeprecatedString;
class StringBuilder;
class StringImpl;
class StringView;
class Time;
class URL;
class FlyString;
class String;
class Utf16View;
class Utf32View;
class Utf8CodePointIterator;
class Utf8View;
class InputStream;
class InputMemoryStream;
class DuplexMemoryStream;
class OutputStream;
class InputBitStream;
class OutputBitStream;
class OutputMemoryStream;

template<size_t Capacity>
class CircularDuplexStream;

template<typename T>
class Span;

template<typename T, size_t Size>
struct Array;

template<typename Container, typename ValueType>
class SimpleIterator;

using ReadonlyBytes = Span<u8 const>;
using Bytes = Span<u8>;

template<typename T, AK::MemoryOrder DefaultMemoryOrder>
class Atomic;

template<typename T, typename TSizeCalculationPolicy = DefaultSizeCalculationPolicy>
class SinglyLinkedList;

template<typename T>
class DoublyLinkedList;

template<typename T, size_t capacity>
class CircularQueue;

template<typename T>
struct Traits;

template<typename T, typename TraitsForT = Traits<T>, bool IsOrdered = false>
class HashTable;

template<typename T, typename TraitsForT = Traits<T>>
using OrderedHashTable = HashTable<T, TraitsForT, true>;

template<typename K, typename V, typename KeyTraits = Traits<K>, typename ValueTraits = Traits<V>, bool IsOrdered = false>
class HashMap;

template<typename K, typename V, typename KeyTraits = Traits<K>, typename ValueTraits = Traits<V>>
using OrderedHashMap = HashMap<K, V, KeyTraits, ValueTraits, true>;

template<typename T>
class Badge;

template<typename T>
class FixedArray;

template<size_t precision, typename Underlying = i32>
class FixedPoint;

template<typename>
class Function;

template<typename Out, typename... In>
class Function<Out(In...)>;

template<typename T>
class NonnullRefPtr;

template<typename T>
class NonnullOwnPtr;

template<typename T, size_t inline_capacity = 0>
class NonnullOwnPtrVector;

template<typename T, size_t inline_capacity = 0>
class NonnullRefPtrVector;

template<typename T>
class Optional;

#ifdef KERNEL
template<typename T>
class NonnullLockRefPtr;

template<typename T, size_t inline_capacity = 0>
class NonnullLockRefPtrVector;

template<typename T>
struct LockRefPtrTraits;

template<typename T, typename PtrTraits = LockRefPtrTraits<T>>
class LockRefPtr;
#endif

template<typename T>
class RefPtr;

template<typename T, typename TDeleter = DefaultDelete<T>>
class OwnPtr;

template<typename T>
class WeakPtr;

template<typename T, size_t inline_capacity = 0>
requires(!IsRvalueReference<T>) class Vector;

template<typename T, typename ErrorType = Error>
class [[nodiscard]] ErrorOr;

}

#if USING_AK_GLOBALLY
using AK::Array;
using AK::Atomic;
using AK::Badge;
using AK::Bitmap;
using AK::ByteBuffer;
using AK::Bytes;
using AK::CircularBuffer;
using AK::CircularDuplexStream;
using AK::CircularQueue;
using AK::DeprecatedString;
using AK::DoublyLinkedList;
using AK::DuplexMemoryStream;
using AK::Error;
using AK::ErrorOr;
using AK::FixedArray;
using AK::FixedPoint;
using AK::FlyString;
using AK::Function;
using AK::GenericLexer;
using AK::HashMap;
using AK::HashTable;
using AK::InputBitStream;
using AK::InputMemoryStream;
using AK::InputStream;
using AK::IPv4Address;
using AK::JsonArray;
using AK::JsonObject;
using AK::JsonValue;
using AK::NonnullOwnPtr;
using AK::NonnullOwnPtrVector;
using AK::NonnullRefPtr;
using AK::NonnullRefPtrVector;
using AK::Optional;
using AK::OutputBitStream;
using AK::OutputMemoryStream;
using AK::OutputStream;
using AK::OwnPtr;
using AK::ReadonlyBytes;
using AK::RefPtr;
using AK::SinglyLinkedList;
using AK::Span;
using AK::StackInfo;
using AK::String;
using AK::StringBuilder;
using AK::StringImpl;
using AK::StringView;
using AK::Time;
using AK::Traits;
using AK::URL;
using AK::Utf16View;
using AK::Utf32View;
using AK::Utf8CodePointIterator;
using AK::Utf8View;
using AK::Vector;

#    ifdef KERNEL
using AK::LockRefPtr;
using AK::LockRefPtrTraits;
using AK::NonnullLockRefPtr;
using AK::NonnullLockRefPtrVector;
#    endif

#endif
