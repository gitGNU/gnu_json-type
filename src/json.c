// Copyright (C) 2016  Stefan Vargyas
// 
// This file is part of Json-Type.
// 
// Json-Type is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Json-Type is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Json-Type.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"

#include "obj.h"
#include "common.h"

int main(int argc, char* argv[])
{
    const struct options_t* opts = options(argc, argv);
    const struct obj_type_t* type = get_obj_type(opts->object);
    int r;

    OBJ_DECL(obj, type);

    type->init(obj, type, opts);
    r = type->run(obj);
    type->done(obj);

    return r;
}

