#pragma once

#include "defines.h"
#include "math/math_types.h"
#include "resources/resource_types.h"

typedef enum renderer_backend_type {
    RENDERER_BACKEND_TYPE_VULKAN,
    RENDERER_BACKEND_TYPE_OPENGL,
    RENDERER_BACKEND_TYPE_DIRECTX
} renderer_backend_type;

typedef struct geometry_render_data {
    mat4 model;
    geometry* geometry;
} geometry_render_data;

typedef enum builtin_renderpass {
    BUILTIN_RENDERPASS_WORLD = 0x01,
    BUILTIN_RENDERPASS_UI = 0x02
} builtin_renderpass;

/**
 * @brief A generic "interface" for the backend. The renderer backend
 * is what is responsible for making calls to the graphics API such as
 * Vulkan, OpenGL or DirectX. Each of these should implement this interface.
 * The frontend only interacts via this structure and has no knowledge of
 * the way things actually work on the backend.
 */
typedef struct renderer_backend {
    u64 frame_number;

    /**
     * @brief Initializes the backend.
     * 
     * @param backend A pointer to the generic backend interface.
     * @param application_name The name of the application.
     * @return True if initialized successfully; otherwise false.
     */
    b8 (*initialize)(struct renderer_backend* backend, const char* application_name);

    /**
     * @brief Shuts the renderer backend down.
     * 
     * @param backend A pointer to the generic backend interface.
     */
    void (*shutdown)(struct renderer_backend* backend);

    /**
     * @brief Handles window resizes.
     * 
     * @param backend A pointer to the generic backend interface.
     * @param width The new window width.
     * @param height The new window height.
     */
    void (*resized)(struct renderer_backend* backend, u16 width, u16 height);

    /**
     * @brief Performs setup routines required at the start of a frame.
     * @note A false result does not necessarily indicate failure. It can also specify that
     * the backend is simply not in a state capable of drawing a frame at the moment, and 
     * that it should be attempted again on the next loop. End frame does not need to (and
     * should not) be called if this is the case.
     * @param backend A pointer to the generic backend interface.
     * @param delta_time The time in seconds since the last frame.
     * @return True if successful; otherwise false. 
     */
    b8 (*begin_frame)(struct renderer_backend* backend, f32 delta_time);

    /**
     * @brief Updates global state items, such as view and projection. Should only be called
     * while in the world renderpass.
     * 
     * @param projection The projection matrix to be set.
     * @param view The view matrix to be set.
     * @param view_position The view position (camera position) to be set.
     * @param ambient_colour THe ambient world colour.
     * @param mode The render mode.
     */
    void (*update_global_world_state)(mat4 projection, mat4 view, vec3 view_position, vec4 ambient_colour, i32 mode);

    /**
     * @brief Updates global state items for the UI, such as view and projection. Should only be
     * called while in the UI renderpass.
     * 
     * @param projection The projection matrix to be set.
     * @param view The view matrix to be set.
     * @param mode The render mode.
     */
    void (*update_global_ui_state)(mat4 projection, mat4 view, i32 mode);

    /**
     * @brief Performs routines required to draw a frame, such as presentation. Should only be called
     * after a successful return of begin_frame.
     * 
     * @param backend A pointer to the generic backend interface.
     * @param delta_time The time in seconds since the last frame.
     * @return True on success; otherwise false.
     */
    b8 (*end_frame)(struct renderer_backend* backend, f32 delta_time);

    /**
     * @brief Begins a renderpass with the given id.
     * 
     * @param backend A pointer to the generic backend interface.
     * @param renderpass_id The identifier of the renderpass to begin.
     * @return True on success; otherwise false.
     */
    b8 (*begin_renderpass)(struct renderer_backend* backend, u8 renderpass_id);

    /**
     * @brief Ends a renderpass with the given id.
     * 
     * @param backend A pointer to the generic backend interface.
     * @param renderpass_id The identifier of the renderpass to end.
     * @return True on success; otherwise false.
     */
    b8 (*end_renderpass)(struct renderer_backend* backend, u8 renderpass_id);

    /**
     * @brief Draws the given geometry. Should only be called inside a renderpass, within a frame.
     * 
     * @param data The render data of the geometry to be drawn.
     */
    void (*draw_geometry)(geometry_render_data data);

    /**
     * @brief Creates a Vulkan-specific texture, acquiring internal resources as needed.
     * 
     * @param pixels The raw image data used for the texture.
     * @param texture A pointer to the texture to hold the resources.
     */
    void (*create_texture)(const u8* pixels, struct texture* texture);

    /**
     * @brief Destroys the given texture, releasing internal resources.
     * 
     * @param texture A pointer to the texture to be destroyed.
     */
    void (*destroy_texture)(struct texture* texture);

    /**
     * @brief Creates a material, acquiring required internal resources.
     * 
     * @param material A pointer to the material to hold the resources.
     * @return True on success; otherwise false.
     */
    b8 (*create_material)(struct material* material);

    /**
     * @brief Destroys a texture, releasing required internal resouces.
     * 
     * @param material A pointer to the material whose resources should be released.
     */
    void (*destroy_material)(struct material* material);

    /**
     * @brief Creates Vulkan-specific internal resources for the given geometry using
     * the data provided.
     * 
     * @param geometry A pointer to the geometry to be created.
     * @param vertex_size The size of a single vertex.
     * @param vertex_count The total number of vertices.
     * @param vertices An array of vertices.
     * @param index_size The size of an individual index.
     * @param index_count The total number of indices.
     * @param indices An array of indices.
     * @return True on success; otherwise false.
     */
    b8 (*create_geometry)(geometry* geometry, u32 vertex_size, u32 vertex_count, const void* vertices, u32 index_size, u32 index_count, const void* indices);
    
    /**
     * @brief Destroys the given geometry, releasing internal resources.
     * 
     * @param geometry A pointer to the geometry to be destroyed.
     */
    void (*destroy_geometry)(geometry* geometry);
} renderer_backend;

typedef struct render_packet {
    f32 delta_time;

    u32 geometry_count;
    geometry_render_data* geometries;

    u32 ui_geometry_count;
    geometry_render_data* ui_geometries;
} render_packet;