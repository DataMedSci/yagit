#include <math/gamma_index.hpp>
#include <random>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <image.h>
#include <solver.h>
#include <CL/cl.h>

using namespace yagit::core::math;
using namespace yagit::core::data;
using namespace std;
namespace fs = std::filesystem;
using namespace yagit::core::math::execution;



cl_platform_id platform;
cl_device_id device;
cl_context context;
cl_command_queue command_queue;
cl_program program;
cl_kernel local_f64_kernels[3];
cl_kernel global_f64_kernels[3];

string load_all(const fs::path& path)
{
	ifstream input(path);
	stringstream buffer;
	buffer << input.rdbuf();
	return buffer.str();
}

const char* getErrorString(cl_int error)
{
	switch (error) {
		// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

		// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

		// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
}

bool check(cl_int errc)
{
	if (errc)
		cout << "Error [code=" << errc << "]: " << getErrorString(errc) << endl;
	return errc;
}

cl_platform_id select_platform()
{
	cl_uint num_platforms;
	if (check(clGetPlatformIDs(0, nullptr, &num_platforms)))
		return 0;
	vector<cl_platform_id> platforms = vector<cl_platform_id>(num_platforms);
	if (check(clGetPlatformIDs(num_platforms, platforms.data(), nullptr)))
		return 0;
	for (auto& platform : platforms)
	{
		size_t len;
		if (check(clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &len)))
			return 0;
		unique_ptr<char[]> name = make_unique<char[]>(len + 1);
		name[len] = '\0';
		if (check(clGetPlatformInfo(platform, CL_PLATFORM_NAME, len, name.get(), nullptr)))
			return 0;
		cout << "Platform: " << name << endl;
	}
	return platforms.empty() ? 0 : platforms.front();
}

cl_device_id select_device()
{
	cl_uint num_devices;
	if (check(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices)))
		return 0;
	vector<cl_device_id> devices = vector<cl_device_id>(num_devices);
	if (check(clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices.data(), nullptr)))
		return 0;
	for (auto& device : devices)
	{
		size_t len;
		if (check(clGetDeviceInfo(device, CL_DEVICE_NAME, 0, nullptr, &len)))
			return 0;
		unique_ptr<char[]> name = make_unique<char[]>(len + 1);
		name[len] = '\0';
		if (check(clGetDeviceInfo(device, CL_DEVICE_NAME, len, name.get(), nullptr)))
			return 0;
		size_t wgsize;
		if (check(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(wgsize), &wgsize, nullptr)))
			return 0;
		size_t wilen;
		if (check(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, nullptr, &wilen)))
			return 0;
		vector<size_t> wisizes = vector<size_t>(wilen);
		if (check(clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, wilen, wisizes.data(), nullptr)))
			return 0;

		cout << "Device: " << name << ", WG size: " << wgsize << endl;
		cout << "WG item sizes:" << endl;
		for (auto& wisize : wisizes)
			cout << wisize << endl;


	}
	return devices.empty() ? 0 : devices.front();
}

void context_notify(const char* error, const void* pi, size_t cb, void* usrd)
{
	cout << "Context error: " << error << endl;
}

cl_context create_context()
{
	cl_int errc;
	auto context = clCreateContext(nullptr, 1, &device, context_notify, nullptr, &errc);
	if (check(errc))
		return 0;
	return context;
}

cl_command_queue create_command_queue()
{
	cl_int errc;
	auto command_queue = clCreateCommandQueue(context, device, 0, &errc);
	if (check(errc))
		return 0;
	return command_queue;
}

void program_build_notify(cl_program p, void* usrd)
{
	cout << "Program build notification" << endl;
}

cl_program create_and_build_program()
{
	auto program_source = load_all("main.cl");
	auto source_ptr = program_source.c_str();
	cl_int errc;
	auto program = clCreateProgramWithSource(context, 1, &source_ptr, nullptr, &errc);
	if (check(errc))
		return 0;

	const char* options =
		"-cl-mad-enable "
		"-cl-fast-relaxed-math";

	errc = clBuildProgram(program, 0, nullptr, options, program_build_notify, nullptr);
	if (check(errc))
	{
		if (errc == CL_BUILD_PROGRAM_FAILURE)
		{
			size_t len;
			if (check(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &len)))
				return 0;
			unique_ptr<char[]> log = make_unique<char[]>(len + 1);
			log[len] = '\0';
			if (check(clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, len, log.get(), nullptr)))
				return 0;
			cout << "Build Log:" << endl;
			cout << log << endl;

			return 0;
		}
	}

	return program;
}

void create_kernels(cl_kernel(&local_f64_kernels)[3], cl_kernel(&global_f64_kernels)[3])
{
	cl_int errc;

	local_f64_kernels[0] = clCreateKernel(program, "local_gamma_index_1D", &errc);
	if (check(errc))
		return;
	local_f64_kernels[1] = clCreateKernel(program, "local_gamma_index_2D", &errc);
	if (check(errc))
		return;
	local_f64_kernels[2] = clCreateKernel(program, "local_gamma_index_3D", &errc);
	if (check(errc))
		return;

	size_t wisize;
	if (check(clGetKernelWorkGroupInfo(local_f64_kernels[2], device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(wisize), &wisize, nullptr)))
		return;
	cout << "Kernel WG size: " << wisize << endl;

	global_f64_kernels[0] = clCreateKernel(program, "global_gamma_index_1D", &errc);
	if (check(errc))
		return;
	global_f64_kernels[1] = clCreateKernel(program, "global_gamma_index_2D", &errc);
	if (check(errc))
		return;
	global_f64_kernels[2] = clCreateKernel(program, "global_gamma_index_3D", &errc);
	if (check(errc))
		return;
}

cl_mem create_input_image_3D(const sizes<3>& size)
{
	cl_int errc;
	cl_image_format format
	{
		.image_channel_order = CL_R,
		.image_channel_data_type = CL_FLOAT,
	};
	cl_image_desc desc
	{
		.image_type = CL_MEM_OBJECT_IMAGE3D,
		.image_width = size.sizes[0],
		.image_height = size.sizes[1],
		.image_depth = size.sizes[2],
		.image_array_size = 0,
		.image_row_pitch = 0,
		.image_slice_pitch = 0,
		.num_mip_levels = 0,
		.num_samples = 0,
		.buffer = nullptr,
	};

	auto image = clCreateImage(context, CL_MEM_READ_ONLY, &format, &desc, nullptr, &errc);
	if (check(errc))
		return 0;

	return image;
}

cl_mem create_output_image_3D(const sizes<3>& size)
{
	cl_int errc;
	cl_image_format format
	{
		.image_channel_order = CL_R,
		.image_channel_data_type = CL_FLOAT,
	};
	cl_image_desc desc
	{
		.image_type = CL_MEM_OBJECT_IMAGE3D,
		.image_width = size.sizes[0],
		.image_height = size.sizes[1],
		.image_depth = size.sizes[2],
		.image_array_size = 0,
		.image_row_pitch = 0,
		.image_slice_pitch = 0,
		.num_mip_levels = 0,
		.num_samples = 0,
		.buffer = nullptr,
	};

	auto image = clCreateImage(context, CL_MEM_WRITE_ONLY, &format, &desc, nullptr, &errc);
	if (check(errc))
		return 0;

	return image;
}

void copy_to_device(cl_mem image, const float* data, const sizes<3>& size)
{
	size_t origin[3] = { 0,0,0 };
	if (check(clEnqueueWriteImage(command_queue, image, true, origin, size.sizes, 0, 0, data, 0, nullptr, nullptr)))
		return;
}

void copy_from_device(cl_mem image, float* data, const sizes<3>& size)
{
	size_t origin[3] = { 0,0,0 };
	if (check(clEnqueueReadImage(command_queue, image, true, origin, size.sizes, 0, 0, data, 0, nullptr, nullptr)))
		return;
}

template<typename T, size_t D>
array<int, D> to_int(const array<T, D>& vs)
{
	array<int, D> res;
	transform(begin(vs), end(vs), begin(res), [](auto&& v) {return static_cast<int>(v); });
	return res;
}

template<typename T, size_t D>
array<float, D> to_float(const array<T, D>& vs)
{
	array<float, D> res;
	transform(begin(vs), end(vs), begin(res), [](auto&& v) {return static_cast<float>(v); });
	return res;
}

template<typename T, size_t D>
array<T, D> operator*(const array<T, D>& vs1, const array<T, D>& vs2)
{
	array<T, D> res;
	transform(begin(vs1), end(vs1), begin(vs2), begin(res), [](auto&& v1, auto&& v2) {return v1 * v2; });
	return res;
}

template<typename T, size_t D>
array<T, D> operator+(const array<T, D>& vs1, const array<T, D>& vs2)
{
	array<T, D> res;
	transform(begin(vs1), end(vs1), begin(vs2), begin(res), [](auto&& v1, auto&& v2) {return v1 + v2; });
	return res;
}

template<typename T, size_t D>
array<T, D> operator-(const array<T, D>& vs1, const array<T, D>& vs2)
{
	array<T, D> res;
	transform(begin(vs1), end(vs1), begin(vs2), begin(res), [](auto&& v1, auto&& v2) {return v1 - v2; });
	return res;
}

template<typename T, size_t D>
array<T, D> operator/(const array<T, D>& vs1, const array<T, D>& vs2)
{
	array<T, D> res;
	transform(begin(vs1), end(vs1), begin(vs2), begin(res), [](auto&& v1, auto&& v2) {return v1 / v2; });
	return res;
}

template<typename T, size_t D>
array<T, D> ceil(const array<T, D>& vs)
{
	array<T, D> res;
	transform(begin(vs), end(vs), begin(res), [](auto&& v) {return ceil(v); });
	return res;
}


template<typename T, size_t D>
array<T, D> floor(const array<T, D>& vs)
{
	array<T, D> res;
	transform(begin(vs), end(vs), begin(res), [](auto&& v) {return floor(v); });
	return res;
}

template<typename T, size_t D>
array<T, D> min(const array<T, D>& vs1, const array<T, D>& vs2)
{
	array<T, D> res;
	transform(begin(vs1), end(vs1), begin(vs2), begin(res), [](auto&& v1, auto&& v2) {return min(v1, v2); });
	return res;
}

template<typename T, size_t D>
array<T, D> max(const array<T, D>& vs1, const array<T, D>& vs2)
{
	array<T, D> res;
	transform(begin(vs1), end(vs1), begin(vs2), begin(res), [](auto&& v1, auto&& v2) {return max(v1, v2); });
	return res;
}

void debug_logic(float distance_to_agreement, const array<int, 3>& reference_pixel_index, const array<int, 3>& target_size, const array<float, 3>& target_position, const array<float, 3>& target_spacing)
{
	array<float, 3> ref_pos = { 0.0f,0.0f, 0.0f };
	array<float, 3> ref_spac = { 0.1f,0.2f,0.3f };
	float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;

	auto reference_position = ref_pos + to_float(reference_pixel_index) * ref_spac;

	auto target_position_end = target_position + target_spacing * to_float(target_size);
	auto target_step_amount = target_spacing;
	// target pixel index position based on reference position
	auto denormalized_target_initial_position = (reference_position - target_position) / target_spacing;
	// target image normalized position based on reference position
	auto target_initial_position = denormalized_target_initial_position / to_float(target_size);
	// maximum steps in x, y, z (if position difference divided by distance_to_agreement is > 1 then GI value must be > 1 so no need to explore further)
	auto step_count_radius_p = to_int(ceil(array{ distance_to_agreement, distance_to_agreement, distance_to_agreement } / target_step_amount));
	auto step_count_radius_n = step_count_radius_p;
	// clamp step_count_radius_p/_n if it would step out of the image
	auto steps_to_positive_edge = (target_position_end - reference_position) / target_step_amount;
	auto steps_to_negative_edge = (reference_position - target_position) / target_step_amount;
	step_count_radius_p = to_int(ceil(min(steps_to_positive_edge, to_float(step_count_radius_p))));
	step_count_radius_n = to_int(ceil(min(steps_to_negative_edge, to_float(step_count_radius_n))));

	cout << "initial position: ";
	for (auto& px : denormalized_target_initial_position)
	{
		cout << px << ", ";
	}
	cout << "positive: ";
	for (auto& px : step_count_radius_p)
	{
		cout << px << ", ";
	}
	cout << endl;
	cout << "negative: ";
	for (auto& nx : step_count_radius_n)
	{
		cout << nx << ", ";
	}
	cout << endl;
}

void run_3d_gi_fd(cl_mem out, cl_mem ref, cl_mem tar, cl_mem ref_x, cl_mem ref_y, cl_mem ref_z, cl_mem tar_x, cl_mem tar_y, cl_mem tar_z, const array<float, 3>& pos, const array<float, 3>& spacing, const local_gamma_index_params<cl_float>& p, const sizes<3>& size)
{
	if (check(clSetKernelArg(local_f64_kernels[2], 0, sizeof(cl_mem), &out)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 1, sizeof(cl_mem), &ref)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 2, sizeof(cl_mem), &ref_x)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 3, sizeof(cl_mem), &ref_y)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 4, sizeof(cl_mem), &ref_z)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 5, sizeof(cl_mem), &tar)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 6, sizeof(cl_mem), &tar_x)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 7, sizeof(cl_mem), &tar_y)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 8, sizeof(cl_mem), &tar_z)))
		return;
	cl_float3 tar_pos;
	tar_pos.x = pos[0];
	tar_pos.y = pos[1];
	tar_pos.z = pos[2];
	if (check(clSetKernelArg(local_f64_kernels[2], 9, sizeof(cl_float3), &tar_pos)))
		return;
	cl_float3 tar_spac;
	tar_spac.x = spacing[0];
	tar_spac.y = spacing[1];
	tar_spac.z = spacing[2];
	if (check(clSetKernelArg(local_f64_kernels[2], 10, sizeof(cl_float3), &tar_spac)))
		return;
	auto distance_to_agreement = std::sqrt(p.distance_to_agreement_squared);
	if (check(clSetKernelArg(local_f64_kernels[2], 11, sizeof(cl_float), &distance_to_agreement)))
		return;
	if (check(clSetKernelArg(local_f64_kernels[2], 12, sizeof(cl_float), &p.percentage)))
		return;

	debug_logic(distance_to_agreement, { 4,4,4 }, array<int, 3>{(int)size.sizes[0], (int)size.sizes[1], (int)size.sizes[2]}, pos, spacing);
	size_t local_size[3] = { 1, 1, 1 };
	if (check(clEnqueueNDRangeKernel(command_queue, local_f64_kernels[2], 3, nullptr, size.sizes, local_size, 0, nullptr, nullptr)))
		return;
	clFinish(command_queue);
}

float passing_rate(const vector<float>& gi)
{
	size_t count_passed = count_if(std::execution::par_unseq, begin(gi), end(gi), [](auto&& v) {return v <= 1.0f; });
	return static_cast<float>(count_passed) / gi.size();
}

void init_pipeline()
{
	platform = select_platform();
	device = select_device();
	context = create_context();
	command_queue = create_command_queue();
	program = create_and_build_program();
	create_kernels(local_f64_kernels, global_f64_kernels);
	auto image_size = sizes<3>{ 1024, 1024, 1024 };
}

void gen_coords(const array<float, 3>& pos, const array<float, 3>& spacing, float* xs, float* ys, float* zs, const sizes<3>& size)
{
	for (size_t x = 0; x < size.sizes[0]; x++)
	{
		for (size_t y = 0; y < size.sizes[1]; y++)
		{
			for (size_t z = 0; z < size.sizes[2]; z++)
			{
				xs[(x * size.sizes[1] + y) * size.sizes[2] + z] = pos[0] + spacing[0] * x;
				ys[(x * size.sizes[1] + y) * size.sizes[2] + z] = pos[1] + spacing[1] * y;
				zs[(x * size.sizes[1] + y) * size.sizes[2] + z] = pos[2] + spacing[2] * z;
			}
		}
	}
}

void test_gpu_float()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<float> uniform_dist(0.0, 10.0f);

	constexpr size_t SX = 2, SY = 2, SZ = 2;
	constexpr size_t S = SX * SY * SZ;
	constexpr float epsilon = 1e-4;

	vector<float> output_seq(S, 0.0f);
	vector<float> output_unseq(S, 0.0f);
	vector<float> output_par(S, 0.0f);
	vector<float> output_par_unseq(S, 0.0f);

	vector<float> ref_doses(S, 0.0f);
	vector<float> ref_x(S, 0.0f);
	vector<float> ref_y(S, 0.0f);
	vector<float> ref_z(S, 0.0f);
	gen_coords({ 0.0f,0.0f, 0.0f }, { 0.1f,0.2f,0.3f }, ref_x.data(), ref_y.data(), ref_z.data(), { SX, SY, SZ });


	vector<float> tar_doses(S, 0.0f);
	vector<float> tar_x(S, 0.0f);
	vector<float> tar_y(S, 0.0f);
	vector<float> tar_z(S, 0.0f);
	array<float, 3> tar_pos = { 0.0f,0.0f,0.0f };//{ 0.07f,0.02f, 0.05f };
	array<float, 3> tar_spac = { 0.1f,0.2f,0.3f };
	gen_coords(tar_pos, tar_spac, tar_x.data(), tar_y.data(), tar_z.data(), { SX, SY, SZ });

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	copy(begin(ref_doses), end(ref_doses), begin(tar_doses));
	//std::generate_n(tar_doses.data(), S, gen);

	auto start = chrono::high_resolution_clock::now();

	auto imsize = sizes<3>{ SX, SY, SZ };
	auto out = create_output_image_3D(imsize);

	auto rd = create_input_image_3D(imsize);
	auto rx = create_input_image_3D(imsize);
	auto ry = create_input_image_3D(imsize);
	auto rz = create_input_image_3D(imsize);

	auto td = create_input_image_3D(imsize);
	auto tx = create_input_image_3D(imsize);
	auto ty = create_input_image_3D(imsize);
	auto tz = create_input_image_3D(imsize);

	copy_to_device(rd, ref_doses.data(), imsize);
	copy_to_device(rx, ref_x.data(), imsize);
	copy_to_device(ry, ref_y.data(), imsize);
	copy_to_device(rz, ref_z.data(), imsize);

	copy_to_device(td, tar_doses.data(), imsize);
	copy_to_device(tx, tar_x.data(), imsize);
	copy_to_device(ty, tar_y.data(), imsize);
	copy_to_device(tz, tar_z.data(), imsize);

	local_gamma_index_params<cl_float> params{ 0.5, 2.0 };

	run_3d_gi_fd(out, rd, td, rx, ry, rz, tx, ty, tz, tar_pos, tar_spac, params, imsize);

	copy_from_device(out, output_par_unseq.data(), imsize);

	auto end = chrono::high_resolution_clock::now();

	cout << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << 's' << endl;
	cout << "GI passing rate: " << passing_rate(output_par_unseq)*100 << "%" << endl;
}

void test_float()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<float> uniform_dist(0.0, 10.0f);

	constexpr size_t SX = 2, SY = 2, SZ = 2;
	constexpr size_t S = SX * SY * SZ;
	constexpr float epsilon = 1e-4;

	vector<float> output_seq(S, 0.0f);
	vector<float> output_unseq(S, 0.0f);
	vector<float> output_par(S, 0.0f);
	vector<float> output_par_unseq(S, 0.0f);

	vector<float> ref_doses(S, 0.0f);
	vector<float> ref_x(S, 0.0f);
	vector<float> ref_y(S, 0.0f);
	vector<float> ref_z(S, 0.0f);
	gen_coords({ 0.0f,0.0f, 0.0f }, { 0.1f,0.2f,0.3f }, ref_x.data(), ref_y.data(), ref_z.data(), { SX, SY, SZ });


	vector<float> tar_doses(S, 0.0f);
	vector<float> tar_x(S, 0.0f);
	vector<float> tar_y(S, 0.0f);
	vector<float> tar_z(S, 0.0f);
	array<float, 3> tar_pos = { 0.0f,0.0f,0.0f };//{ 0.07f,0.02f, 0.05f };
	array<float, 3> tar_spac = { 0.1f,0.2f,0.3f };
	gen_coords(tar_pos, tar_spac, tar_x.data(), tar_y.data(), tar_z.data(), { SX, SY, SZ });

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	copy(begin(ref_doses), end(ref_doses), begin(tar_doses));
	//std::generate_n(tar_doses.data(), S, gen);

	auto start = chrono::high_resolution_clock::now();

	local_gamma_index_params<float> params{ 0.5f, 1.0f };

	gamma_index(algorithm_version::classic{}, parallel_unsequenced_policy{}, output_par_unseq.data(), output_par_unseq.data() + S,
		ref_doses.data(),
		array<const float*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
		tar_doses.data(), tar_doses.data() + S,
		array<const float*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
		params);

	auto end = chrono::high_resolution_clock::now();

	cout << chrono::duration_cast<chrono::nanoseconds>(end - start).count() / 1e9 << 's' << endl;
	cout << "GI passing rate: " << passing_rate(output_par_unseq) * 100 << "%" << endl;
}

void test_double()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);

	constexpr size_t S = 200 * 80 * 20;
	constexpr double epsilon = 1e-9;

	vector<double> output_seq(S, 0.0);
	vector<double> output_unseq(S, 0.0);
	vector<double> output_par(S, 0.0);
	vector<double> output_par_unseq(S, 0.0);

	vector<double> ref_doses(S, 0.0);
	vector<double> ref_x(S, 0.0);
	vector<double> ref_y(S, 0.0);
	vector<double> ref_z(S, 0.0);

	vector<double> tar_doses(S, 0.0);
	vector<double> tar_x(S, 0.0);
	vector<double> tar_y(S, 0.0);
	vector<double> tar_z(S, 0.0);

	auto gen = [&]() {return uniform_dist(e1); };

	std::generate_n(ref_doses.data(), S, gen);
	std::generate_n(ref_x.data(), S, gen);
	std::generate_n(ref_y.data(), S, gen);
	std::generate_n(ref_z.data(), S, gen);

	std::generate_n(tar_doses.data(), S, gen);
	std::generate_n(tar_x.data(), S, gen);
	std::generate_n(tar_y.data(), S, gen);
	std::generate_n(tar_z.data(), S, gen);

	local_gamma_index_params<double> params{ 0.5f, 1.0f };

	gamma_index(algorithm_version::classic{}, parallel_unsequenced_policy{}, output_par_unseq.data(), output_par_unseq.data() + S,
		ref_doses.data(),
		array<const double*, 3>{ref_x.data(), ref_y.data(), ref_z.data()},
		tar_doses.data(), tar_doses.data() + S,
		array<const double*, 3>{tar_x.data(), tar_y.data(), tar_z.data()},
		params);
}

void test_orig_sr()
{
	random_device r;
	default_random_engine e1(r());
	uniform_real_distribution<double> uniform_dist(0.0, 10.0f);
	auto gen = [&]() {return uniform_dist(e1); };

	constexpr size_t SX = 100;
	constexpr size_t SY = 20;
	constexpr size_t SZ = 10;
	constexpr size_t S = SX * SY * SZ;
	constexpr double epsilon = 1e-9;

	// original format
	vector<double> ref_start = { 0.0,0.0,0.0 };
	vector<double> ref_spacing = { 0.5,0.2,0.3 };
	vector<vector<vector<double>>> ref_data(SX, vector<vector<double>>(SY, vector<double>(SZ, 0.0)));

	for (auto& vv : ref_data)
		for (auto& v : vv)
			for (auto& e : v)
				e = gen();

	Image3D img_ref = Image3D(ref_start, ref_spacing, ref_data);

	vector<double> tar_start = { -0.5,0.3,0.2 };
	vector<double> tar_spacing = { 0.4,0.1,0.4 };
	vector<vector<vector<double>>> tar_data(SX, vector<vector<double>>(SY, vector<double>(SZ, 0.0)));

	for (auto& vv : tar_data)
		for (auto& v : vv)
			for (auto& e : v)
				e = gen();

	Image3D img_tar = Image3D(tar_start, tar_spacing, tar_data);

	SpiralNoRectangleSolver3D solver = SpiralNoRectangleSolver3D(img_ref, img_tar, 0.5f, 0.5f);

	Image3D result = solver.calculateGamma();
}


int main()
{
	init_pipeline();

	string test_type;
	cout << "Select test type [float, float_gpu, double, orig_sr]: ";
	cin >> test_type;

	if (test_type == "float")
	{
		test_float();
	}
	else if (test_type == "float_gpu")
	{
		test_gpu_float();
	}
	else if (test_type == "double")
	{
		test_double();
	}
	else if(test_type == "orig_sr")
	{
		test_orig_sr();
	}
	
	return 0;
}