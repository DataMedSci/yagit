#include <data/image/float64_image.hpp>

namespace yagit::gdcm::data
{
	class float64_image1D::impl
		: ycd::iwritable_image<double, 1>
	{
		friend class float64_image1D;
	public:
		virtual ycd::sizes<1> size() const override;
		virtual ycd::data_format<1> preferred_data_format() const override;
		virtual ycd::data_region<1> region() const noexcept override;
		virtual error_code save(const ycd::image_data<double, 1>& data) override;
	protected:
		virtual error_code load(double* data_storage, const ycd::data_format<1>& format, size_t& required_size) const override;
		virtual unique_ptr<iimage_region<double, 0>> slice(size_t dimension, size_t index, error_code& ec) const override;
        virtual iwritable_image_region<double, 1>* create_subregion(const ycd::data_region<1>& region, error_code& ec) const override;
	};

	class float64_image2D::impl
		: ycd::iwritable_image<double, 2>
	{
		friend class float64_image2D;
	public:
		virtual ycd::sizes<2> size() const override;
		virtual ycd::data_format<2> preferred_data_format() const override;
		virtual ycd::data_region<2> region() const noexcept override;
		virtual error_code save(const ycd::image_data<double, 2>& data) override;
	protected:
		virtual error_code load(double* data_storage, const ycd::data_format<2>& format, size_t& required_size) const override;
		virtual unique_ptr<iimage_region<double, 1>> slice(size_t dimension, size_t index, error_code& ec) const override;
        virtual iwritable_image_region<double, 2>* create_subregion(const ycd::data_region<2>& region, error_code& ec) const override;
    };

	class float64_image3D::impl
		: ycd::iwritable_image<double, 3>
	{
		friend class float64_image3D;
	public:
		virtual ycd::sizes<3> size() const override;
		virtual ycd::data_format<3> preferred_data_format() const override;
		virtual ycd::data_region<3> region() const noexcept override;
		virtual error_code save(const ycd::image_data<double, 3>& data) override;
	protected:
		virtual error_code load(double* data_storage, const ycd::data_format<3>& format, size_t& required_size) const override;
		virtual unique_ptr<iimage_region<double, 2>> slice(size_t dimension, size_t index, error_code& ec) const override;
        virtual iwritable_image_region<double, 3>* create_subregion(const ycd::data_region<3>& region, error_code& ec) const override;
	};
}