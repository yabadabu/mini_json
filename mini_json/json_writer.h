#pragma once

#include <cassert>

namespace MiniJson {

	class JsonWriter;

	template<typename T>
	void save(JsonWriter& b, const T& obj);

	class JsonWriter {

	public:

		template<typename T>
		void operator<<(const T& obj) {
			save(*this, obj);
			++state.nitems;
		}

		void operator<<(const char* txt) {
			addQuoted(txt);
		}

		void operator<<(bool b) {
			addUnquoted(b ? "true" : "false");
		}

		void operator<<(size_t sz) {
			TStr64 s("%llu", sz);
			addUnquoted(s.c_str());
		}

		void operator<<(int n) {
			TStr64 s("%d", n);
			addUnquoted(s.c_str());
		}

		void operator<<(float f) {
			TStr64 s("%f", f);
			addUnquoted(s.c_str());
		}

		void writeFile(const char* ofilename) const {
			assert(stack.empty());
			buf.save(ofilename);
		}

		const char* c_str() {
			if (buf.back() != 0x00) {
				buf.reserve(buf.size() + 1);
				buf.data()[buf.size()] = 0x00;
			}
			return (const char*)buf.data();
		}

		size_t size() const {
			return buf.size();
		}

		struct Obj {
			JsonWriter& jw;
			Obj(JsonWriter& b) : jw(b) {
				jw.beginGroup(eMode::Object);
			}
			~Obj() {
				jw.endGroup();
			}
			template< typename T>
			void prop(const char* key, const T& obj) {
				jw.prop(key, obj);
			}
		};

		struct Arr {
			JsonWriter& jw;
			Arr(JsonWriter& b) : jw(b) {
				jw.beginGroup(eMode::Array);
			}
			~Arr() {
				jw.endGroup();
			}
			template< typename T>
			void operator<<(const T& obj) {
				jw << obj;
			}
			void separator() {
				jw.separator();
			}
		};

		JsonWriter(bool in_skip_formatting = true)
			: skip_formatting(in_skip_formatting) {
		}

	private:

		friend struct Obj;
		friend struct Arr;

		enum class eMode {
			Undefined, Single, Array, Object
		};

		struct State {
			eMode mode = eMode::Undefined;
			int   nitems = 0;
		};

		State         state;
		Vector<State> stack;
		bool          skip_formatting = false;

		TBuffer buf;
		void newLine() {
			if (skip_formatting)
				return;
			buf.append('\n');
			for (size_t k = 0; k < stack.size(); ++k) {
				buf.append(' ');
				buf.append(' ');
			}
		}
		void separator() {
			buf.append(',');
			newLine();
		}

		template< typename T>
		void prop(const char* key, const T& obj) {
			assert(state.mode == eMode::Object);
			if (state.nitems > 0)
				separator();
			buf.append('"');
			buf.append(key, strlen(key));
			buf.append('"');
			buf.append(':');
			State prev_state = state;
			state = {};
			*this << obj;
			state = prev_state;
			state.nitems++;
		}

		void addQuoted(const char* txt) {
			buf.append('"');
			// TODO escape characters
			buf.append(txt, strlen(txt));
			buf.append('"');
			++state.nitems;
		}

		void addUnquoted(const char* txt) {
			buf.append(txt, strlen(txt));
			++state.nitems;
		}

		void beginGroup(eMode mode) {
			assert(mode == eMode::Array || mode == eMode::Object);
			buf.append((mode == eMode::Array) ? '[' : '{');
			stack.push_back(state);
			state.mode = mode;
			state.nitems = 0;
			newLine();
		}

		void endGroup() {
			assert(state.mode == eMode::Array || state.mode == eMode::Object);
			char symbol = (state.mode == eMode::Array) ? ']' : '}';
			bool empty = (state.nitems == 0);
			state = stack.back();
			stack.pop_back();
			if (!empty)
				newLine();
			buf.append(symbol);
		}
	};

	template<typename T, typename TContainer>
	void save(JsonWriter& jw, const TContainer<T>& v) {
		JsonWriter::Arr arr(jw);
		size_t sz = v.size();
		for (size_t i = 0; i < sz; ++i) {
			if (i > 0)
				arr.separator();
			arr << v[i];
		}
	}

}