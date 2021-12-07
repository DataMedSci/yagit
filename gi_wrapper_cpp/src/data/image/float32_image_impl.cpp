#include <data/image/float32_image_impl.hpp>

namespace yagit::gdcm::data
{
	constexpr uint16_t pixel_data_group = 0x7FE0;
	constexpr uint16_t pixel_data_element = 0x0010;

	// -------- float32_image1D --------

	ycd::sizes<1> float32_image1D::size() const
	{
		return _d->size();
	}

	ycd::data_format<1> float32_image1D::preferred_data_format() const
	{
		return _d->preferred_data_format();
	}

	ycd::data_region<1> float32_image1D::region() const noexcept
	{
		return _d->region();
	}

	ycd::iwritable_image_region<float, 1>* float32_image1D::create_subregion(const ycd::data_region<1>& region, error_code& ec) const
	{
		return _d->create_subregion(region, ec);
	}

	error_code float32_image1D::save(const ycd::image_data<float, 1>& data)
	{
		return _d->save(data);
	}

	error_code float32_image1D::load(float* data_storage, const ycd::data_format<1>& format, size_t& required_size) const
	{
		return _d->load(data_storage, format, required_size);
	}

	unique_ptr<ycd::iimage_region<float, 0>> float32_image1D::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return _d->slice(dimension, index, ec);
	}

	// -------- float32_image2D --------

	ycd::sizes<2> float32_image2D::size() const
	{
		return _d->size();
	}

	ycd::data_format<2> float32_image2D::preferred_data_format() const
	{
		return _d->preferred_data_format();
	}

	ycd::data_region<2> float32_image2D::region() const noexcept
	{
		return _d->region();
	}

	ycd::iwritable_image_region<float, 2>* float32_image2D::create_subregion(const ycd::data_region<2>& region, error_code& ec) const
	{
		return _d->create_subregion(region, ec);
	}

	error_code float32_image2D::save(const ycd::image_data<float, 2>& data)
	{
		return _d->save(data);
	}

	error_code float32_image2D::load(float* data_storage, const ycd::data_format<2>& format, size_t& required_size) const
	{
		return _d->load(data_storage, format, required_size);
	}

	unique_ptr<ycd::iimage_region<float, 1>> float32_image2D::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return _d->slice(dimension, index, ec);
	}

	// -------- float32_image3D --------

	ycd::sizes<3> float32_image3D::size() const
	{
		return _d->size();
	}

	ycd::data_format<3> float32_image3D::preferred_data_format() const
	{
		return _d->preferred_data_format();
	}

	ycd::data_region<3> float32_image3D::region() const noexcept
	{
		return _d->region();
	}

	ycd::iwritable_image_region<float, 3>* float32_image3D::create_subregion(const ycd::data_region<3>& region, error_code& ec) const
	{
		return _d->create_subregion(region, ec);
	}

	error_code float32_image3D::save(const ycd::image_data<float, 3>& data)
	{
		return _d->save(data);
	}

	error_code float32_image3D::load(float* data_storage, const ycd::data_format<3>& format, size_t& required_size) const
	{
		return _d->load(data_storage, format, required_size);
	}

	unique_ptr<ycd::iimage_region<float, 2>> float32_image3D::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return _d->slice(dimension, index, ec);
	}

	// -------- float32_image1D::impl --------

	float32_image1D::impl::impl(::gdcm::Image& image, const ycd::sizes<1>& size)
		: _image(image)
	{
		_image.SetNumberOfDimensions(1);
		_image.SetDimension(0, size.sizes[0]);
		::gdcm::PixelFormat pf = ::gdcm::PixelFormat::FLOAT32;
		pf.SetSamplesPerPixel(1);
		image.SetPixelFormat(pf);
		::gdcm::PhotometricInterpretation pi = ::gdcm::PhotometricInterpretation::MONOCHROME2;
		_image.SetPhotometricInterpretation(pi);
		_image.SetTransferSyntax(::gdcm::TransferSyntax::ExplicitVRLittleEndian);
	}

	ycd::sizes<1> float32_image1D::impl::size() const
	{
		return ycd::sizes<1>{_image.GetDimension(0)};
	}

	ycd::data_format<1> float32_image1D::impl::preferred_data_format() const
	{
		return ycd::default_format<1>;
	}

	ycd::data_region<1> float32_image1D::impl::region() const noexcept
	{
		return ycd::data_region<1>{ {0}, size()};
	}

	error_code float32_image1D::impl::save(const ycd::image_data<float, 1>& data)
	{
		::gdcm::DataElement pixel_data{ ::gdcm::Tag(pixel_data_group, pixel_data_element) };
		
		pixel_data.SetByteValue(
			reinterpret_cast<const char*>(data.data()),
			static_cast<uint32_t>(data.total_size())
		);

		_image.SetDataElement(pixel_data);

		return {}; //success
	}

	error_code float32_image1D::impl::load(float* data_storage, const ycd::data_format<1>& format, size_t& required_size) const
	{
		if (format != preferred_data_format())
			return {}; //error

		const auto& data_element = _image.GetDataElement();
		const auto& byte_value = data_element.GetByteValue();

		if (byte_value == nullptr)
			return {}; // error

		if (data_storage == nullptr)
		{
			required_size = byte_value->GetLength();
		}
		else
		{
			if(required_size < byte_value->GetLength())
				return {}; // error

			byte_value->GetBuffer(reinterpret_cast<char*>(data_storage), byte_value->GetLength());
		}

		return {}; //success
	}

	unique_ptr<ycd::iimage_region<float, 0>> float32_image1D::impl::slice(size_t dimension, size_t index, error_code& ec) const
	{
		return nullptr;
	}
}