/*
Copyright (c) 2019-2020 Péter Magyar

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef VOXELMAN_LIBRARY_SIMPLE_H
#define VOXELMAN_LIBRARY_SIMPLE_H

#include "voxelman_library.h"

#include "core/resource.h"
#include "scene/resources/material.h"

#include "../data/voxel_light.h"
#include "voxel_surface_simple.h"

class VoxelSurfaceSimple;
class VoxelMesher;

class VoxelmanLibrarySimple : public VoxelmanLibrary {
	GDCLASS(VoxelmanLibrarySimple, VoxelmanLibrary)

public:
	int get_atlas_columns() const;
	void set_atlas_columns(int s);

	int get_atlas_rows() const;
	void set_atlas_rows(int s);

	Ref<VoxelSurface> get_voxel_surface(const int index);
	void add_voxel_surface(Ref<VoxelSurface> value);
	void set_voxel_surface(const int index, Ref<VoxelSurface> value);
	void remove_surface(const int index);
	int get_num_surfaces() const;
	void clear_surfaces();

	Vector<Variant> get_voxel_surfaces();
	void set_voxel_surfaces(const Vector<Variant> &surfaces);

	void refresh_rects();

	VoxelmanLibrarySimple();
	~VoxelmanLibrarySimple();

protected:
	static void _bind_methods();

private:
	Vector<Ref<VoxelSurfaceSimple> > _voxel_surfaces;

	//atlas
	int _atlas_columns;
	int _atlas_rows;
};

#endif // VOXEL_LIBRARY_H
