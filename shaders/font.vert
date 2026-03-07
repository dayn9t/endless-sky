/* font.vert
Copyright (c) 2014-2020 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

// scale maps pixel coordinates to GL coordinates (-1 to 1).
uniform vec2 scale;
// The (x, y) coordinates of the top left corner of the glyph.
uniform vec2 position;
// UV rectangle in atlas: (u0, v0, u1, v1)
uniform vec4 uvRect;
// Glyph size (in pixels).
uniform vec2 glyphSize;

// Inputs from the VBO.
in vec2 vert;
in vec2 corner;

// Output to the fragment shader.
out vec2 texCoord;

void main() {
	texCoord = vec2(
		uvRect.x + corner.x * (uvRect.z - uvRect.x),
		uvRect.y + corner.y * (uvRect.w - uvRect.y)
	);
	vec2 pos = vert * glyphSize;
	gl_Position = vec4(
		(pos.x + position.x) * scale.x,
		(pos.y + position.y) * scale.y,
		0.f, 1.f
	);
}
