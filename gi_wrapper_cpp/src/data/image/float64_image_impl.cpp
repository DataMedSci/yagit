#include <data/image/float64_image_impl.hpp>

namespace yagit::gdcm::data
{
	// -------- float64_image1D --------

	ycd::sizes<1> float64_image1D::size() const
	{
		return _d->size();
	}

	ycd::data_format<1> float64_image1D::preferred_data_format() const
	{
		return _d->preferred_data_format();
	}

	ycd::data_region<1> float64_image1D::region() const noexcept
	{
		return _d->region();
	}

	ycd::iwritable_image_region<double, 1>* float64_image1D::create_subregion(const ycd::data_region<1> &region, error_code &ec) const
	{
		return _d->create_subregion(region, ec);
	}

	error_code float64_image1D::save(const ycd::image_data<double, 1>& data)
	{
		return _d->save(data);
	}

	error_code float64_image1D::load(double* data_storage, const ycd::data_format<1>& format, size_t& required_size) const
	{
		return _d->load(data_storage, format, required_size);
	}

	unique_ptr<ycd::iimage_region<double, 0>> float64_image1D::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return _d->slice(dimension, index, ec);
	}

	// -------- float64_image2D --------

	ycd::sizes<2> float64_image2D::size() const
	{
		return _d->size();
	}

	ycd::data_format<2> float64_image2D::preferred_data_format() const
	{
		return _d->preferred_data_format();
	}

	ycd::data_region<2> float64_image2D::region() const noexcept
	{
		return _d->region();
	}

	ycd::iwritable_image_region<double, 2>* float64_image2D::create_subregion(const ycd::data_region<2>& region, error_code& ec) const
	{
		return _d->create_subregion(region, ec);
	}

	error_code float64_image2D::save(const ycd::image_data<double, 2>& data)
	{
		return _d->save(data);
	}

	error_code float64_image2D::load(double* data_storage, const ycd::data_format<2>& format, size_t& required_size) const
	{
		return _d->load(data_storage, format, required_size);
	}

	unique_ptr<ycd::iimage_region<double, 1>> float64_image2D::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return _d->slice(dimension, index, ec);
	}

	// -------- float64_image3D --------

	ycd::sizes<3> float64_image3D::size() const
	{
		return _d->size();
	}

	ycd::data_format<3> float64_image3D::preferred_data_format() const
	{
		return _d->preferred_data_format();
	}

	ycd::data_region<3> float64_image3D::region() const noexcept
	{
		return _d->region();
	}

	ycd::iwritable_image_region<double, 3>* float64_image3D::create_subregion(const ycd::data_region<3>& region, error_code& ec) const
	{
		return _d->create_subregion(region, ec);
	}

	error_code float64_image3D::save(const ycd::image_data<double, 3>& data)
	{
		return _d->save(data);
	}

	error_code float64_image3D::load(double* data_storage, const ycd::data_format<3>& format, size_t& required_size) const
	{
		return _d->load(data_storage, format, required_size);
	}

	unique_ptr<ycd::iimage_region<double, 2>> float64_image3D::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return _d->slice(dimension, index, ec);
	}
}