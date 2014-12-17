// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

float fbm( vec3 p, vec3 n )
{
	p *= 0.15;

	float x = texture2D( iChannel3, p.yz ).x;
	float y = texture2D( iChannel3, p.zx ).x;
	float z = texture2D( iChannel3, p.xy ).x;

	return x*abs(n.x) + y*abs(n.y) + z*abs(n.z);
}

vec2 distanceToSegmentb( vec3 a, vec3 b, vec3 p )
{
	vec3 pa = p - a;
	vec3 ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
	
	return vec2( length( pa - ba*h ), h );
}

vec3 hash3( float n )
{
    return fract(sin(vec3(n,n+1.0,n+2.0))*vec3(43758.5453123,22578.1459123,19642.3490423));
}

vec3 snoise3( in float x )
{
    float p = floor(x);
    float f = fract(x);

    f = f*f*(3.0-2.0*f);

    return -1.0 + 2.0*mix( hash3(p+0.0), hash3(p+1.0), f );
}

float freqs[16];

vec4 map( vec3 pos )
{
	pos += 0.04*sin(10.0*pos.yzx);
	
	vec3 qpos = mod( 1000.0 + pos+1.0, 2.0 )-1.0;
	
    vec3 off3 = floor( 1000.0 + (pos+1.0)/2.0 );

	qpos *= sign( cos( 0.5*3.1415927*pos.yzx ) );
	
	float off = abs( dot( off3, vec3(1.0, 13.0, 7.0 ) ) );
		
	float mindist = 10000.0;
	vec3 p = vec3(0.0);
	float h = 0.0;
	float rad = 0.04 + 0.15*freqs[0];
	float mint = 0.0;
    for( int i=0; i<16; i++ )
	{
		vec3 op = p;
		
		p  = 0.9*normalize(snoise3( 8.0*h ));

		float orad = rad;
		rad = (0.04 + 0.15*freqs[i])*1.5*1.1;
		
		vec2 disl = distanceToSegmentb( op, p, qpos );
		float t = h + disl.y/16.0;

		float dis = disl.x - mix(orad,rad,disl.y);
		
		if( dis<mindist ){ mindist = dis; mint=t; }
		h += (1.0/16.0);
	}

	float dsp = sin(50.0*pos.x)*sin(50.0*pos.y)*sin(50.0*pos.z);
	dsp = dsp*dsp*dsp;
	mindist += -0.02*dsp;
	
	mindist += 0.01*sin(180.0*mint + iGlobalTime);
	
	
    return vec4(mindist,1.0,dsp,mint);
}




vec4 castRay( in vec3 ro, in vec3 rd, in float precis, in float maxd )
{
    float h=precis*2.0;
    vec3 c;
    float t = 0.0;
    float sid = -1.0;
	float dsp = 0.0;
	float ttt = 0.0;
    for( int i=0; i<48; i++ )
    {
        if( abs(h)<precis||t>maxd ) continue;// break;
        t += min( h, 0.2 );
	    vec4 res = map( ro+rd*t );
        h = res.x;
	    sid = res.y;
		dsp = res.z;
		ttt = res.w;
    }

    if( t>maxd ) sid=-1.0;
    return vec4( t, sid, dsp, ttt );
}



vec3 calcNormal( in vec3 pos )
{
	vec3 eps = vec3( 0.05, 0.0, 0.0 );
	vec3 nor = vec3(
	    map(pos+eps.xyy).x - map(pos-eps.xyy).x,
	    map(pos+eps.yxy).x - map(pos-eps.yxy).x,
	    map(pos+eps.yyx).x - map(pos-eps.yyx).x );
	return normalize(nor);
}

vec3 doBumpMap( in vec3 pos, in vec3 nor, float amount )
{
    float e = 0.0015;
    float b = 0.01;
    
	float ref = fbm( 48.0*pos, nor );
    vec3 gra = -b*vec3( fbm(48.0*vec3(pos.x+e, pos.y, pos.z),nor)-ref,
                        fbm(48.0*vec3(pos.x, pos.y+e, pos.z),nor)-ref,
                        fbm(48.0*vec3(pos.x, pos.y, pos.z+e),nor)-ref )/e;
	
	vec3 tgrad = gra - nor * dot ( nor , gra );
    return normalize ( nor - amount*tgrad );

	
}

float calcAO( in vec3 pos, in vec3 nor )
{

    float totao = 0.0;
    for( int aoi=0; aoi<5; aoi++ )
    {
		vec3 aopos = 0.1 * ( nor  + 0.7*(-1.0+2.0*hash3(143.13*float(aoi))) );
        float dd = map( pos + aopos ).x;
		totao += clamp(5.0*dd,0.0,1.0);
    }
    return pow( clamp( 1.5*totao/5.0, 0.0, 1.0 ), 1.0 );
}


float calcSSS( in vec3 pos, in vec3 nor )
{
    float ao = 1.0;
    float totao = 0.0;
    float sca = 1.0;
    for( int aoi=0; aoi<5; aoi++ )
    {
        float hr = 0.01 + 0.4*float(aoi)/4.0;
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ).x;
        totao += (hr-min(dd,0.0))*sca;
        sca *= 0.9;
    }
    return pow( clamp( 1.2 - 0.25*totao, 0.0, 1.0 ), 16.0 );
}


vec3 render( in vec3 ro, in vec3 rd )
{ 
    vec3 col = vec3(0.0);

	vec4 res = castRay(ro,rd,0.001,8.0);
    float t = res.x;
    if( res.y>-0.5 )
    {
        vec3 pos = ro + t*rd;
        vec3 nor = calcNormal( pos );
		vec3 snor = nor;
		nor = doBumpMap( 0.5*pos, nor, 0.2*clamp(1.0-1.0*res.z,0.0,1.0) );

		vec3 ref = reflect( rd, nor );
		vec3 sref = reflect( rd, snor );

		col = vec3(1.0);
		float pao = calcAO( pos, nor ); 
        float ao = 0.4 + 0.6*pao;
        ao *= 1.0 + 0.3*max(res.z,0.0);
		float ss = calcSSS( pos-nor*0.01, rd ); 
  
		float kr = clamp( 1.0+dot( rd, nor ), 0.0, 1.0 );

		col = mix( vec3(0.6,0.3,0.1), 1.4*vec3(1.0,0.8,0.6), kr*ss*ss );

        col *= 0.7 + 0.3*ss;
		
		col += 0.1*cos( 4.0*6.2831*res.w + vec3(1.0,0.5,0.7) );
        col *= 0.6 + 0.6*fbm(pos,nor);		
        col = col*ao;
		col += 0.15*(0.5+0.5*kr)*pow(textureCube( iChannel1, normalize(sref+ref) ).xyz, vec3(1.0) );

		col *= exp( -0.0125*t*t*t );
    }
   
	return vec3( col );
}

void main( void )
{
	vec2 q = gl_FragCoord.xy/iResolution.xy;
    vec2 p = -1.0+2.0*q;
	p.x *= iResolution.x/iResolution.y;
    vec2 mo = iMouse.xy/iResolution.xy;
		 
	float time = iGlobalTime + 80.0;

	for( int i=0; i<16; i++ )
	    freqs[i] = clamp( 1.9*pow( texture2D( iChannel0, vec2( 0.02 + 0.5*float(i)/16.0, 0.25 ) ).x, 3.0 ), 0.0, 1.0 );
	
	// camera	
	vec3  ta  = 0.4*vec3( cos(0.115*time), 2.0*sin(0.1*time), sin(0.085*time) );
	vec3  ro = vec3( 1.0*cos(0.05*time+6.28*mo.x), 0.0, 1.0*sin(0.05*time+6.2831*mo.x) );
	float rl = 0.25*sin(0.1*time);
	
	// camera tx
	vec3 cw = normalize( ta-ro );
	vec3 cp = vec3( sin(rl), cos(rl),0.0 );
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
	vec3 rd = normalize( p.x*cu + p.y*cv + 1.5*cw );

    vec3 col = render( ro, rd );

	col *= 0.25+0.75*pow( 16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y), 0.15 );

    gl_FragColor=vec4( col, 1.0 );
}
