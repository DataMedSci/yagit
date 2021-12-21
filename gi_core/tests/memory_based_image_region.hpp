#pragma once

#include <data/iimage_region.hpp>
#include <algorithm>
#include <numeric>

using namespace yagit::core::data;

enum class mbi_subregion_error
{
	invalid_subregion,
	subregion_outside_bounds,
	unknown
};

namespace std
{
	template <>
	struct is_error_code_enum<mbi_subregion_error> : true_type {};
}

namespace 
{
	struct mbi_subregion_error_category : std::error_category
	{
		const char* name() const noexcept override {
			return "memory_based_image_subregion_error";
		}
		std::string message(int ev) const override
		{
			switch (static_cast<mbi_subregion_error>(ev))
			{
			case mbi_subregion_error::invalid_subregion:
				return "invalid_subregion";
			case mbi_subregion_error::subregion_outside_bounds:
				return "subregion_outside_bounds";
			default:
			case mbi_subregion_error::unknown:
				return "unknown";
			}
		}
	};
}

const mbi_subregion_error_category _mbi_subregion_error_cat{};

template<size_t Dimensions>
constexpr array<size_t, Dimensions> make_index_transform(const sizes<Dimensions>& sizes, const data_format<Dimensions>& target_format = default_format)
{
	static_assert(Dimensions >= 1);
	std::array<size_t, N> index_transform{};
	index_transform[target_format.order.back().dimension_index] = 1;

	size_t partial_sum = sizes[target_format.order.back().dimension_index];
	for (
		auto format_order_element_it = target_format.order.rbegin() + 1;
		format_order_element_it != target_format.order.rend();
		partial_sum *= sizes[format_order_element_it->dimension_index])
	{
		index_transform[format_order_element_it->dimension_index] = partial_sum;
	}
	return index_transform;
}

template<size_t Dimensions>
constexpr size_t calculate_index(const array<size_t, Dimensions>& index, const array<size_t, Dimensions>& index_transform)
{
	return std::transform_reduce(std::begin(index), std::end(index), std::begin(index_transform), static_cast<size_t>(0));
}

template<typename ElementType, size_t Dimensions>
class memory_based_image_region
	: iimage_region<ElementType, Dimensions>
{
	static_assert(Dimensions <= 3, "Dimensions higher that 3 are not supported!");
protected:
	std::shared_ptr<ElementType[]> _data;
	sizes<Dimensions> _size;
	data_region<Dimensions> _region;
protected:
	memory_based_image_region(
		const std::shared_ptr<ElementType[]>& data,
		const sizes<Dimensions>& size,
		const data_region<Dimensions>& region)
		: _data(data)
		, _size(size)
		, _region(region)
	{
	}
private:
	constexpr static std::error_code make_error_code(mbi_subregion_error e)
	{
		return { static_cast<int>(e), _mbi_subregion_error_cat };
	}
public:
	virtual ~memory_based_image_region() override = default;
public:
	virtual data_format<Dimensions> preferred_data_format() const override { return default_format; }
	virtual data_format<Dimensions> preferred_coordinates_format() const override { return default_format; }
	virtual unique_ptr<iimage_region<ElementType, Dimensions>> subregion(const data_region<Dimensions>& region, error_code& ec) const
	{
		if (std::any_of(std::begin(region.size.sizes), std::end(region.size.sizes), [](auto s) {return s == 0; }))
		{
			ec = make_error_code(mbi_subregion_error::invalid_subregion);
			return nullptr;
		}

		data_region<Dimensions> new_region;
		std::transform(std::begin(_region.offset.offsets), std::end(_region.offset.offsets), std::begin(region.offset.offsets), std::begin(new_region.offset.offsets), std::plus<size_t>());
		std::copy(std::begin(region.size.sizes), std::end(region.size.sizes), std::begin(new_region.size.sizes));

		std::array<size_t, Dimensions> upper_bound_region;
		std::array<size_t, Dimensions> upper_bound_new_region;
		std::transform(std::begin(_region.offset.offsets), std::end(_region.offset.offsets), std::begin(_region.size.sizes), std::begin(upper_bound_region), std::plus<size_t>());
		std::transform(std::begin(new_region.offset.offsets), std::end(new_region.offset.offsets), std::begin(new_region.size.sizes), std::begin(upper_bound_new_region), std::plus<size_t>());
		std::array<bool, Dimensions> upper_bound_in_old_region;
		std::transform(std::begin(upper_bound_new_region), std::end(upper_bound_new_region), std::begin(upper_bound_region), std::begin(upper_bound_in_old_region), std::less_equal<size_t>());
		
		if (!std::all_of(std::begin(upper_bound_in_old_region), std::end(upper_bound_in_old_region), [](auto in) {return in; }))
		{
			ec = make_error_code(mbi_subregion_error::invalid_subregion);
			return nullptr;
		}

		ec = error_code();
		return unique_ptr<iimage_region<ElementType, Dimensions>>(new memory_based_image_region(_data, _size, new_region));
	}
	virtual data_region<Dimensions> region() const noexcept { return _region; }
	virtual error_code save(const image_data<ElementType, Dimensions>& data) override
	{
		if (data.region() != region())
			return error_code();/*error*/;

		if (data.format() == preferred_data_format())
		{
			if constexpr (Dimensions == 1)
			{
				std::copy(data.data(), data.data() + data.total_size(), _data.get() + _region.offset.offsets[0]);
			}

			if constexpr (Dimensions >= 2)
			{
				size_t to_row_length = _size.sizes[Dimensions - 1];
				size_t from_row_length = data.size<Dimensions - 1>();
				size_t from_size = data.total_size();
				for (
					auto from = data.data(),
					auto from_end = data.data() + from_size,
					to = _data.get() + _region.offset.offsets[Dimensions - 1];
					from != from_end;
					from += from_row_length;
					to += to_row_length)
				{
					std::copy(from, from + from_row_length, to);
				}
			}

			return error_code();
		}
	}
protected:
	virtual error_code load(ElementType* data_storage, const data_format<Dimensions>& format, size_t& required_size) const override;
	virtual error_code load_coordinates(ElementType* data_storage, size_t dimension, const data_format<Dimensions>& format, size_t& required_size) const override;
	virtual unique_ptr<iimage_region<ElementType, Dimensions - 1>> slice(size_t dimension, size_t index, error_code& ec) const override;
};
