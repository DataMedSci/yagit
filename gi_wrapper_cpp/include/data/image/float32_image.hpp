#include <data/image/iwritable_image.hpp>
#include <type_traits>

namespace yagit::gdcm::data
{
	namespace ycd = yagit::core::data;
	using std::error_code;
	using std::unique_ptr;

	class float32_image1D;
	class float32_image2D;
	class float32_image3D;

	template<size_t Dimensions>
	using float32_image = 
		std::conditional_t<
			Dimensions == 1, float32_image1D,
		std::conditional_t<
			Dimensions == 2, float32_image2D,
		std::conditional_t<
			Dimensions == 3, float32_image3D,
		void
		>>>;

	class float32_image1D
		: ycd::iwritable_image<float, 1>
	{
	private:
		class impl;
		unique_ptr<impl> _d;
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

	class float32_image2D
		: ycd::iwritable_image<float, 2>
	{
	private:
		class impl;
		unique_ptr<impl> _d;
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

	class float32_image3D
		: ycd::iwritable_image<float, 3>
	{
	private:
		class impl;
		unique_ptr<impl> _d;
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