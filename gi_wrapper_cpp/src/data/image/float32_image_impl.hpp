#include <data/image/float32_image.hpp>
#include <gdcmImage.h>

namespace yagit::gdcm::data
{
	class float32_image1D::impl
		: ycd::iwritable_image<float, 1>
	{
		friend class float32_image1D;
	private:
		::gdcm::Image& _image;
	public:
		impl(::gdcm::Image& image, const ycd::sizes<1>& size);
	public:
		virtual ycd::sizes<1> size() const override;
		virtual ycd::data_format<1> preferred_data_format() const override;
		virtual ycd::data_region<1> region() const noexcept override;
		virtual error_code save(const ycd::image_data<float, 1>& data) override;
	protected:
		virtual error_code load(float* data_storage, const ycd::data_format<1>& format, size_t& required_size) const override;
		virtual unique_ptr<iimage_region<float, 0>> slice(size_t dimension, size_t index, error_code& ec) const override;
        virtual iwritable_image_region<float, 1>* create_subregion(const ycd::data_region<1>& region, error_code& ec) const override;
    };

	class float32_image2D::impl
		: ycd::iwritable_image<float, 2>
	{
		friend class float32_image2D;
	public:
		virtual ycd::sizes<2> size() const override;
		virtual ycd::data_format<2> preferred_data_format() const override;
		virtual ycd::data_region<2> region() const noexcept override;
		virtual error_code save(const ycd::image_data<float, 2>& data) override;
	protected:
		virtual error_code load(float* data_storage, const ycd::data_format<2>& format, size_t& required_size) const override;
		virtual unique_ptr<iimage_region<float, 1>> slice(size_t dimension, size_t index, error_code& ec) const override;
        virtual iwritable_image_region<float, 2>* create_subregion(const ycd::data_region<2>& region, error_code& ec) const override;
	};

	class float32_image3D::impl
		: ycd::iwritable_image<float, 3>
	{
		friend class float32_image3D;
	public:
		virtual ycd::sizes<3> size() const override;
		virtual ycd::data_format<3> preferred_data_format() const override;
		virtual ycd::data_region<3> region() const noexcept override;
		virtual error_code save(const ycd::image_data<float, 3>& data) override;
	protected:
		virtual error_code load(float* data_storage, const ycd::data_format<3>& format, size_t& required_size) const override;
		virtual unique_ptr<iimage_region<float, 2>> slice(size_t dimension, size_t index, error_code& ec) const override;
        virtual iwritable_image_region<float, 3>* create_subregion(const ycd::data_region<3>& region, error_code& ec) const override;
	};
}