#ifndef LAK_BIGINT_HPP
#define LAK_BIGINT_HPP

#include "lak/compare.hpp"
#include "lak/defer.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	struct bigint
	{
		using div_rem_result = lak::pair<bigint, bigint>;

	private:
		bool _negative = false;
		std::vector<uintmax_t> _data;

		bigint &negate();
		void reserve(size_t count);
		void normalise(size_t min_count = 0U);
		size_t min_size() const;

		lak::span<const uintmax_t> min_span() const;

		// ignores _negative
		void add(uintmax_t value);
		void sub(uintmax_t value); // must not overflow
		[[nodicard]] div_rem_result div_rem_impl(uintmax_t value,
		                                         bool negate_result) const;
		void add(const bigint &value);
		void sub(const bigint &value); // must not overflow
		[[nodicard]] div_rem_result div_rem_impl(const bigint &value,
		                                         bool negate_result) const;

	public:
		bigint()               = default;
		bigint(const bigint &) = default;
		bigint(bigint &&)      = default;
		bigint &operator=(const bigint &) = default;
		bigint &operator=(bigint &&) = default;

		bigint(uintmax_t value);
		bigint(intmax_t value);
		bigint(unsigned value) : bigint(uintmax_t(value)) {}
		bigint(signed value) : bigint(intmax_t(value)) {}

		bigint &operator=(uintmax_t value);
		bigint &operator=(intmax_t value);
		bigint &operator=(unsigned value) { return *this = uintmax_t(value); }
		bigint &operator=(signed value) { return *this = intmax_t(value); }

		lak::result<uintmax_t> to_uintmax() const;
		lak::result<intmax_t> to_intmax() const;
		double to_double() const;

		bool is_negative() const;
		bool is_positive() const { return !is_negative(); }
		bool is_zero() const;
		bool is_big() const;

		uintmax_t bit_count() const;
		uintmax_t min_bit_count() const;
		unsigned bit(uintmax_t index) const;
		void set_bit(uintmax_t index, unsigned value);

		/* --- uintmax_t --- */

		[[nodiscard]] div_rem_result div_rem(uintmax_t rhs) const;

		bigint &operator+=(uintmax_t rhs);
		bigint &operator-=(uintmax_t rhs);
		bigint &operator*=(uintmax_t rhs);
		bigint &operator/=(uintmax_t rhs);
		bigint &operator%=(uintmax_t rhs);
		bigint &operator<<=(uintmax_t rhs);
		bigint &operator>>=(uintmax_t rhs);

		bigint operator+(uintmax_t rhs) &&;
		bigint operator-(uintmax_t rhs) &&;
		bigint operator*(uintmax_t rhs) &&;
		bigint operator/(uintmax_t rhs) &&;
		bigint operator%(uintmax_t rhs) &&;
		bigint operator<<(uintmax_t rhs) &&;
		bigint operator>>(uintmax_t rhs) &&;

		bigint operator+(uintmax_t rhs) const &;
		bigint operator-(uintmax_t rhs) const &;
		bigint operator*(uintmax_t rhs) const &;
		bigint operator/(uintmax_t rhs) const &;
		bigint operator%(uintmax_t rhs) const &;
		bigint operator<<(uintmax_t rhs) const &;
		bigint operator>>(uintmax_t rhs) const &;

		lak::strong_ordering operator<=>(uintmax_t rhs) const;
		bool operator<(uintmax_t rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(uintmax_t rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(uintmax_t rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(uintmax_t rhs) const { return operator<=>(rhs) != 0; }

		/* --- intmax_t --- */

		[[nodiscard]] div_rem_result div_rem(intmax_t rhs) const;

		bigint &operator+=(intmax_t rhs);
		bigint &operator-=(intmax_t rhs);
		bigint &operator*=(intmax_t rhs);
		bigint &operator/=(intmax_t rhs);
		bigint &operator%=(intmax_t rhs);

		bigint operator+(intmax_t rhs) &&;
		bigint operator-(intmax_t rhs) &&;
		bigint operator*(intmax_t rhs) &&;
		bigint operator/(intmax_t rhs) &&;
		bigint operator%(intmax_t rhs) &&;

		bigint operator+(intmax_t rhs) const &;
		bigint operator-(intmax_t rhs) const &;
		bigint operator*(intmax_t rhs) const &;
		bigint operator/(intmax_t rhs) const &;
		bigint operator%(intmax_t rhs) const &;

		lak::strong_ordering operator<=>(intmax_t rhs) const;
		bool operator<(intmax_t rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(intmax_t rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(intmax_t rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(intmax_t rhs) const { return operator<=>(rhs) != 0; }

		/* --- unsigned --- */

		bigint &operator+=(unsigned rhs) { return operator+=(uintmax_t(rhs)); }
		bigint &operator-=(unsigned rhs) { return operator-=(uintmax_t(rhs)); }
		bigint &operator*=(unsigned rhs) { return operator*=(uintmax_t(rhs)); }
		bigint &operator/=(unsigned rhs) { return operator/=(uintmax_t(rhs)); }
		bigint &operator%=(unsigned rhs) { return operator%=(uintmax_t(rhs)); }
		bigint &operator<<=(unsigned rhs) { return operator<<=(uintmax_t(rhs)); }
		bigint &operator>>=(unsigned rhs) { return operator>>=(uintmax_t(rhs)); }

		bigint operator+(unsigned rhs) && { return operator+(uintmax_t(rhs)); }
		bigint operator-(unsigned rhs) && { return operator-(uintmax_t(rhs)); }
		bigint operator*(unsigned rhs) && { return operator*(uintmax_t(rhs)); }
		bigint operator/(unsigned rhs) && { return operator/(uintmax_t(rhs)); }
		bigint operator%(unsigned rhs) && { return operator%(uintmax_t(rhs)); }
		bigint operator<<(unsigned rhs) && { return operator<<(uintmax_t(rhs)); }
		bigint operator>>(unsigned rhs) && { return operator>>(uintmax_t(rhs)); }

		bigint operator+(unsigned rhs) const &
		{
			return operator+(uintmax_t(rhs));
		}
		bigint operator-(unsigned rhs) const &
		{
			return operator-(uintmax_t(rhs));
		}
		bigint operator*(unsigned rhs) const &
		{
			return operator*(uintmax_t(rhs));
		}
		bigint operator/(unsigned rhs) const &
		{
			return operator/(uintmax_t(rhs));
		}
		bigint operator%(unsigned rhs) const &
		{
			return operator%(uintmax_t(rhs));
		}
		bigint operator<<(unsigned rhs) const &
		{
			return operator<<(uintmax_t(rhs));
		}
		bigint operator>>(unsigned rhs) const &
		{
			return operator>>(uintmax_t(rhs));
		}

		lak::strong_ordering operator<=>(unsigned rhs) const
		{
			return operator<=>(uintmax_t(rhs));
		}
		bool operator<(unsigned rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(unsigned rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(unsigned rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(unsigned rhs) const { return operator<=>(rhs) != 0; }

		/* --- signed --- */

		bigint &operator+=(signed rhs) { return operator+=(intmax_t(rhs)); }
		bigint &operator-=(signed rhs) { return operator-=(intmax_t(rhs)); }
		bigint &operator*=(signed rhs) { return operator*=(intmax_t(rhs)); }
		bigint &operator/=(signed rhs) { return operator/=(intmax_t(rhs)); }
		bigint &operator%=(signed rhs) { return operator%=(intmax_t(rhs)); }

		bigint operator+(signed rhs) && { return operator+(intmax_t(rhs)); }
		bigint operator-(signed rhs) && { return operator-(intmax_t(rhs)); }
		bigint operator*(signed rhs) && { return operator*(intmax_t(rhs)); }
		bigint operator/(signed rhs) && { return operator/(intmax_t(rhs)); }
		bigint operator%(signed rhs) && { return operator%(intmax_t(rhs)); }

		bigint operator+(signed rhs) const & { return operator+(intmax_t(rhs)); }
		bigint operator-(signed rhs) const & { return operator-(intmax_t(rhs)); }
		bigint operator*(signed rhs) const & { return operator*(intmax_t(rhs)); }
		bigint operator/(signed rhs) const & { return operator/(intmax_t(rhs)); }
		bigint operator%(signed rhs) const & { return operator%(intmax_t(rhs)); }

		lak::strong_ordering operator<=>(signed rhs) const
		{
			return operator<=>(intmax_t(rhs));
		}
		bool operator<(signed rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(signed rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(signed rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(signed rhs) const { return operator<=>(rhs) != 0; }

		/* --- bigint --- */

		[[nodiscard]] div_rem_result div_rem(const bigint &rhs) const;

		bigint &operator+=(const bigint &rhs);
		bigint &operator-=(const bigint &rhs);
		bigint &operator*=(const bigint &rhs);
		bigint &operator/=(const bigint &rhs);
		bigint &operator%=(const bigint &rhs);
		bigint &operator<<=(const bigint &rhs);
		bigint &operator>>=(const bigint &rhs);

		bigint operator+(bigint &&rhs) const &;
		bigint operator*(bigint &&rhs) const &;

		bigint operator+(bigint &&rhs) && { return operator+(rhs); }
		bigint operator*(bigint &&rhs) && { return operator*(rhs); }

		bigint operator+(const bigint &rhs) &&;
		bigint operator-(const bigint &rhs) &&;
		bigint operator*(const bigint &rhs) &&;
		bigint operator/(const bigint &rhs) &&;
		bigint operator%(const bigint &rhs) &&;
		bigint operator<<(const bigint &rhs) &&;
		bigint operator>>(const bigint &rhs) &&;

		bigint operator+(const bigint &rhs) const &;
		bigint operator-(const bigint &rhs) const &;
		bigint operator*(const bigint &rhs) const &;
		bigint operator/(const bigint &rhs) const &;
		bigint operator%(const bigint &rhs) const &;
		bigint operator<<(const bigint &rhs) const &;
		bigint operator>>(const bigint &rhs) const &;

		lak::strong_ordering operator<=>(const bigint &rhs) const;
		bool operator<(const bigint &) const  = default;
		bool operator>(const bigint &) const  = default;
		bool operator==(const bigint &) const = default;
		bool operator!=(const bigint &) const = default;

		/* --- unary --- */

		bigint operator-() const &;
		bigint operator-() &&;

		friend std::ostream &operator<<(std::ostream &strm, const lak::bigint &val)
		{
			const auto old_width = strm.width();
			DEFER(strm.width(old_width));
			const auto old_base = strm.flags(std::ostream::basefield);
			DEFER(strm.setf(old_base, std::ostream::basefield));

			strm << (val.is_negative() ? '-' : '+');
			strm << std::hex << std::setfill('0');

			for (size_t i = val._data.size(); i-- > 0U;)
			{
				strm << std::setw(sizeof(uintmax_t) * 2U) << val._data[i];
				if (i != 0U) strm << '\'';
			}

			return strm;
		}
	};
}

#endif
