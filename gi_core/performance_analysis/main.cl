#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__constant sampler_t sampler_interp = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_LINEAR;

float distance_to_agreement_1D(
    float reference_position, float target_position, 
    float distance_to_agreement_sq
)
{
    float diff = target_position - reference_position;
    return dot(diff, diff) / distance_to_agreement_sq;
}

float distance_to_agreement_2D(
    float2 reference_position, float2 target_position,
    float distance_to_agreement_sq
)
{
    float2 diff = target_position - reference_position;
    return dot(diff, diff) / distance_to_agreement_sq;
}

float distance_to_agreement_3D(
    float3 reference_position, float3 target_position,
    float distance_to_agreement_sq
)
{
    float3 diff = target_position - reference_position;
    return dot(diff, diff) / distance_to_agreement_sq;
}

float local_dose_difference(
    float reference, float target,
    float percentage
)
{
    float intermediate = (target - reference) / (reference * percentage);
    return intermediate * intermediate;
}

float global_dose_difference(
    float reference, float target,
    float dose_difference_sq
)
{
    float intermediate = target - reference;
    return intermediate * intermediate / dose_difference_sq;
}

float local_gamma_index_1D_single(
    float reference, float reference_position,
    float target, float target_position,
    float distance_to_agreement_sq,
    float percentage
)
{
    return local_dose_difference(reference, target, percentage) + distance_to_agreement_3D(reference_position, target_position, distance_to_agreement_sq);
}

float local_gamma_index_2D_single(
    float reference, float2 reference_position,
    float target, float2 target_position,
    float distance_to_agreement_sq,
    float percentage
)
{
    return local_dose_difference(reference, target, percentage) + distance_to_agreement_2D(reference_position, target_position, distance_to_agreement_sq);
}

float local_gamma_index_3D_single(
    float reference, float3 reference_position,
    float target, float3 target_position,
    float distance_to_agreement_sq,
    float percentage
)
{
    return local_dose_difference(reference, target, percentage) + distance_to_agreement_3D(reference_position, target_position, distance_to_agreement_sq);
}

float global_gamma_index_1D_single(
    float reference, float reference_position,
    float target, float target_position,
    float distance_to_agreement_sq,
    float dose_difference_sq
)
{
    return global_dose_difference(reference, target, dose_difference_sq) + distance_to_agreement_3D(reference_position, target_position, distance_to_agreement_sq);
}

float global_gamma_index_2D_single(
    float reference, float2 reference_position,
    float target, float2 target_position,
    float distance_to_agreement_sq,
    float dose_difference_sq
)
{
    return global_dose_difference(reference, target, dose_difference_sq) + distance_to_agreement_2D(reference_position, target_position, distance_to_agreement_sq);
}

float global_gamma_index_3D_single(
    float reference, float3 reference_position,
    float target, float3 target_position,
    float distance_to_agreement_sq,
    float dose_difference_sq
)
{
    return global_dose_difference(reference, target, dose_difference_sq) + distance_to_agreement_3D(reference_position, target_position, distance_to_agreement_sq);
}

__kernel void local_gamma_index_1D(
    write_only image1d_t gamma_index_output,
    read_only image1d_t reference,
    read_only image1d_t reference_x,
    read_only image1d_t target,
    read_only image1d_t target_x,
    float distance_to_agreement,
    float percentage
)
{
    int reference_pixel_index = get_global_id(0);

    float reference_dose = read_imagef(reference, reference_pixel_index).x;
    float reference_position = read_imagef(reference_x, reference_pixel_index).x;

    float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;

    int target_size = get_image_width(target);
    float min_gamma = 1e9;
    for (int x = 0; x < target_size; ++x)
    {
        int target_pixel_index = x;
        float target_dose = read_imagef(target, target_pixel_index).x;
        float target_position = read_imagef(target_x, target_pixel_index).x;

        min_gamma = min(min_gamma, local_gamma_index_1D_single(
            reference_dose, reference_position,
            target_dose, target_position,
            distance_to_agreement_sq, percentage)
        );
    }

    write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
}

__kernel void local_gamma_index_2D(
    write_only image2d_t gamma_index_output,
    read_only image2d_t reference,
    read_only image2d_t reference_x,
    read_only image2d_t reference_y,
    read_only image2d_t target,
    read_only image2d_t target_x,
    read_only image2d_t target_y,
    float distance_to_agreement,
    float percentage
)
{
    int2 reference_pixel_index = (int2)(get_global_id(0), get_global_id(1));

    float reference_dose = read_imagef(reference, reference_pixel_index).x;
    float2 reference_position = (float2)(
        read_imagef(reference_x, reference_pixel_index).x,
        read_imagef(reference_y, reference_pixel_index).x
    );

    float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;

    int2 target_size = (int2)(get_image_width(target), get_image_height(target));
    float min_gamma = 1e9;
    for (int x = 0; x < target_size.x; ++x)
    {
        for (int y = 0; y < target_size.y; ++y)
        {
            int2 target_pixel_index = (int2)(x, y);
            float target_dose = read_imagef(target, target_pixel_index).x;
            float2 target_position = (float2)(
                read_imagef(target_x, target_pixel_index).x,
                read_imagef(target_y, target_pixel_index).x
                );

            min_gamma = min(min_gamma, local_gamma_index_2D_single(
                reference_dose, reference_position,
                target_dose, target_position,
                distance_to_agreement_sq, percentage)
            );
        }
    }

    write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
}

__kernel void local_gamma_index_3D(
    write_only image3d_t gamma_index_output,
    read_only image3d_t reference,
    read_only image3d_t reference_x,
    read_only image3d_t reference_y,
    read_only image3d_t reference_z,
    read_only image3d_t target,
    read_only image3d_t target_x,
    read_only image3d_t target_y,
    read_only image3d_t target_z,
    float3 target_position,
    float3 target_spacing,
    float distance_to_agreement,
    float percentage
)
{
    float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;
    int4 reference_pixel_index = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);

    float reference_dose = read_imagef(reference, reference_pixel_index).x;
    if (fabs(reference_dose) < 0.001f)
    {
        write_imagef(gamma_index_output, reference_pixel_index, (float4)(0, 0, 0, 0));
        return;
    }
    float3 reference_position = (float3)(
        read_imagef(reference_x, reference_pixel_index).x,
        read_imagef(reference_y, reference_pixel_index).x,
        read_imagef(reference_z, reference_pixel_index).x
        );

    int3 target_size = (int3)(get_image_width(target), get_image_height(target), get_image_depth(target));

    float3 target_position_end = target_position + target_spacing * convert_float3(target_size);
    float3 target_step_amount = target_spacing;
    // target pixel index position based on reference position
    float3 denormalized_target_initial_position = (reference_position - target_position) / target_spacing;
    // target image normalized position based on reference position
    float3 target_initial_position = denormalized_target_initial_position / convert_float3(target_size);
    // maximum steps in x, y, z (if position difference divided by distance_to_agreement is > 1 then GI value must be > 1 so no need to explore further)
    int3 step_count_radius_p = convert_int3(ceil(distance_to_agreement / target_step_amount));
    int3 step_count_radius_n = step_count_radius_p;
    // clamp step_count_radius_p/_n if it would step out of the image
    float3 steps_to_positive_edge = (target_position_end - reference_position) / target_step_amount;
    float3 steps_to_negative_edge = (reference_position - target_position) / target_step_amount;
    step_count_radius_p = convert_int3(ceil(min(steps_to_positive_edge, convert_float3(step_count_radius_p))));
    step_count_radius_n = convert_int3(ceil(min(steps_to_negative_edge, convert_float3(step_count_radius_n))));
    
    // start at corresponding position in target image
    // then if not passed (gi <= 1) explore elements in the
    // radius
    float min_gamma = local_gamma_index_3D_single(
        reference_dose, reference_position,
        read_imagef(target, sampler_interp, (float4)(target_initial_position, 0)).x, target_initial_position,
        distance_to_agreement_sq, percentage);
    if (min_gamma < 1)
    {
        write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
    }
    else
    {
        for (int x = -step_count_radius_n.x; x <= step_count_radius_p.x; ++x)
        {
            for (int y = -step_count_radius_n.y; y <= step_count_radius_p.y; ++y)
            {
                for (int z = -step_count_radius_n.z; z <= step_count_radius_p.z; ++z)
                {
                    float3 target_pos = ((float3)(x, y, z)) * target_step_amount + reference_position;
                    float3 target_denormalized_pos = (target_pos - target_position) / target_spacing;
                    float3 target_normalized_pos = target_denormalized_pos / convert_float3(target_size);
                    float target_dose = read_imagef(target, sampler_interp, (float4)(target_normalized_pos, 0)).x;
                    min_gamma = min(min_gamma, local_gamma_index_3D_single(
                        reference_dose, reference_position,
                        target_dose, target_pos,
                        distance_to_agreement_sq, percentage)
                    );
                    if (min_gamma < 1)
                    {
                        write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
                        return;
                    }
                }
            }
        }
    }

    write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
}

__kernel void global_gamma_index_1D(
    write_only image1d_t gamma_index_output,
    read_only image1d_t reference,
    read_only image1d_t reference_x,
    read_only image1d_t target,
    read_only image1d_t target_x,
    float distance_to_agreement,
    float dose_difference
)
{
    int reference_pixel_index = get_global_id(0);

    float reference_dose = read_imagef(reference, reference_pixel_index).x;
    float reference_position = read_imagef(reference_x, reference_pixel_index).x;

    float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;
    float dose_difference_sq = dose_difference * dose_difference;

    int target_size = get_image_width(target);
    float min_gamma = 1e9;
    for (int x = 0; x < target_size; ++x)
    {
        int target_pixel_index = x;
        float target_dose = read_imagef(target, target_pixel_index).x;
        float target_position = read_imagef(target_x, target_pixel_index).x;

        min_gamma = min(min_gamma, global_gamma_index_1D_single(
            reference_dose, reference_position,
            target_dose, target_position,
            distance_to_agreement_sq, dose_difference_sq)
        );
    }

    write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
}

__kernel void global_gamma_index_2D(
    write_only image2d_t gamma_index_output,
    read_only image2d_t reference,
    read_only image2d_t reference_x,
    read_only image2d_t reference_y,
    read_only image2d_t target,
    read_only image2d_t target_x,
    read_only image2d_t target_y,
    float distance_to_agreement,
    float dose_difference
)
{
    int2 reference_pixel_index = (int2)(get_global_id(0), get_global_id(1));

    float reference_dose = read_imagef(reference, reference_pixel_index).x;
    float2 reference_position = (float2)(
        read_imagef(reference_x, reference_pixel_index).x,
        read_imagef(reference_y, reference_pixel_index).x
        );

    float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;
    float dose_difference_sq = dose_difference * dose_difference;

    int2 target_size = (int2)(get_image_width(target), get_image_height(target));
    float min_gamma = 1e9;
    for (int x = 0; x < target_size.x; ++x)
    {
        for (int y = 0; y < target_size.y; ++y)
        {
            int2 target_pixel_index = (int2)(x, y);
            float target_dose = read_imagef(target, target_pixel_index).x;
            float2 target_position = (float2)(
                read_imagef(target_x, target_pixel_index).x,
                read_imagef(target_y, target_pixel_index).x
                );

            min_gamma = min(min_gamma, global_gamma_index_2D_single(
                reference_dose, reference_position,
                target_dose, target_position,
                distance_to_agreement_sq, dose_difference_sq)
            );
        }
    }

    write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
}

__kernel void global_gamma_index_3D(
    write_only image3d_t gamma_index_output,
    read_only image3d_t reference,
    read_only image3d_t reference_x,
    read_only image3d_t reference_y,
    read_only image3d_t reference_z,
    read_only image3d_t target,
    read_only image3d_t target_x,
    read_only image3d_t target_y,
    read_only image3d_t target_z,
    float distance_to_agreement,
    float dose_difference
)
{
    int4 reference_pixel_index = (int4)(get_global_id(0), get_global_id(1), get_global_id(2), 0);

    float reference_dose = read_imagef(reference, reference_pixel_index).x;
    float3 reference_position = (float3)(
        read_imagef(reference_x, reference_pixel_index).x,
        read_imagef(reference_y, reference_pixel_index).x,
        read_imagef(reference_z, reference_pixel_index).x
        );

    float distance_to_agreement_sq = distance_to_agreement * distance_to_agreement;
    float dose_difference_sq = dose_difference * dose_difference;

    int3 target_size = (int3)(get_image_width(target), get_image_height(target), get_image_depth(target));
    float min_gamma = 1e9;
    for (int x = 0; x < target_size.x; ++x)
    {
        for (int y = 0; y < target_size.y; ++y)
        {
            for (int z = 0; z < target_size.z; ++z)
            {
                int4 target_pixel_index = (int4)(x, y, z, 0);
                float target_dose = read_imagef(target, target_pixel_index).x;
                float3 target_position = (float3)(
                    read_imagef(target_x, target_pixel_index).x,
                    read_imagef(target_y, target_pixel_index).x,
                    read_imagef(target_z, target_pixel_index).x
                    );

                min_gamma = min(min_gamma, global_gamma_index_3D_single(
                    reference_dose, reference_position,
                    target_dose, target_position,
                    distance_to_agreement_sq, dose_difference_sq)
                );
            }
        }
    }

    write_imagef(gamma_index_output, reference_pixel_index, (float4)(min_gamma, 0, 0, 0));
}