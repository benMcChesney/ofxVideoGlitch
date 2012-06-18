/*
 *  ofxAlphaMask
 *
 * Example by James George, http://www.jamesgeorge.org
 * in collaboration with FlightPhase http://www.flightphase.com
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * ----------------------
 *
 * ofxAlphaMask is not really an addon, but an example
 * of how to use a shader to have one image become the alpha
 * channel of another.
 */
 
uniform sampler2DRect Tex0 ;

uniform float time ; 
uniform float globalSound ;
uniform float soundMultiplier ; 

uniform float rOffset ; 
uniform float gOffset ; 
uniform float bOffset ;

void main (void)
{
	//sample both textures
    vec2 texPos = gl_TexCoord[0].st ; 
    texPos.x += tan( time ) * globalSound * soundMultiplier ; 
    texPos.y += cos( time * cos ( texPos.x ))  * globalSound * soundMultiplier ; 
	vec4 source = texture2DRect(Tex0, texPos );
	
	//use the color from the image, but use the r channel of the mask as the alpha channel of our output
	gl_FragData[0] = vec4( source.r + rOffset , source.g + gOffset , source.b + bOffset , source.a ) ; //vec4(image.rgb,composite.r);  
	
}
