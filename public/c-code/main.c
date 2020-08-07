#include <stdlib.h>
#include <math.h>
#include "esUtil.h"

typedef struct
{
    // Handle to a program object
    GLuint programObject;

} UserData;

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
    GLuint shader;
    GLint compiled;
    
    // Create the shader object
    shader = glCreateShader ( type );

    if ( shader == 0 )
        return 0;

    // Load the shader source
    glShaderSource ( shader, 1, &shaderSrc, NULL );
    
    // Compile the shader
    glCompileShader ( shader );

    // Check the compile status
    glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

    if ( !compiled ) 
    {
        GLint infoLen = 0;

        glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
        
        if ( infoLen > 1 )
        {
            char* infoLog = malloc (sizeof(char) * infoLen );

            glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
            esLogMessage ( "Error compiling shader:\n%s\n", infoLog );            
            
            free ( infoLog );
        }

        glDeleteShader ( shader );
        return 0;
    }

    return shader;

}

///
// Initialize the shader and program object
//
int Init ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));

    UserData *userData = esContext->userData;
    GLbyte vShaderStr[] =  
        "attribute vec4 vPosition;    \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = vPosition;  \n"
        "}                            \n";
    
    GLbyte fShaderStr[] =  
        "precision mediump float;\n"\
        "void main()                                  \n"
        "{                                            \n"
        "  gl_FragColor = vec4 ( gl_FragCoord.x/1000.0, gl_FragCoord.y/1000.0, 1.0, 1.0 );\n"
        "}                                            \n";

    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
    fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

    // Create the program object
    programObject = glCreateProgram ( );
    
    if ( programObject == 0 )
        return 0;

    glAttachShader ( programObject, vertexShader );
    glAttachShader ( programObject, fragmentShader );

    // Bind vPosition to attribute 0   
    glBindAttribLocation ( programObject, 0, "vPosition" );

    // Link the program
    glLinkProgram ( programObject );

    // Check the link status
    glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

    if ( !linked ) 
    {
        GLint infoLen = 0;

        glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
        
        if ( infoLen > 1 )
        {
            char* infoLog = malloc (sizeof(char) * infoLen );

            glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
            esLogMessage ( "Error linking program:\n%s\n", infoLog );            
            
            free ( infoLog );
        }

        glDeleteProgram ( programObject );
        return GL_FALSE;
    }

    // Store the program object
    userData->programObject = programObject;

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
    return GL_TRUE;
}

///
// Draw a triangle using the shader pair created in Init()
//
void DrawTriangle ( ESContext *esContext,  GLfloat vVertices[9])
{
    glBufferData(GL_ARRAY_BUFFER, 9*4, vVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0 /* ? */, 3, GL_FLOAT, 0, 0, 0);
    
    glDrawArrays ( GL_TRIANGLES, 0, 3 );
}

struct point
{
    double x;
    double y;
    double z;
};

struct polPoint{
    double r;
    double zen;
    double az;
};

void DrawQuadrangle( ESContext *esContext,  GLfloat vVertices[12])
{
    GLfloat vVertices1[] = {  vVertices[0],  vVertices[1], vVertices[2], 
                            vVertices[3], vVertices[4], vVertices[5],
                                vVertices[9], vVertices[10], vVertices[11] };
    DrawTriangle(esContext, vVertices1);
    GLfloat vVertices2[] = { vVertices[3], vVertices[4], vVertices[5],
                                vVertices[6],  vVertices[7], vVertices[8],
                                vVertices[9], vVertices[10], vVertices[11] };
    DrawTriangle(esContext,vVertices2);
}

void DrawQuadranglePoints( ESContext *esContext,  struct point vertices[4])
{
    GLfloat vVertices[] = {
        vertices[0].x, vertices[0].y, vertices[0].z,
        vertices[1].x, vertices[1].y, vertices[1].z,
        vertices[2].x, vertices[2].y, vertices[2].z,
        vertices[3].x, vertices[3].y, vertices[3].z,
    };
    DrawQuadrangle(esContext, vVertices);
}

void multipleMatrix(struct point* p, float matrix[9])
{
    float fp[] = {p->x, p->y, p->z};
    float resault[3] = {};
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            resault[i] += fp[i] * matrix[3 * i + j]; 
        }
    }
    p->x = resault[0];
    p->y = resault[1];
    p->z = resault[2];
}

void rotate(struct point vertices[8], float rotateMatrix[9])
{
    for (int i = 0; i < 8; i++){
        multipleMatrix(vertices + i, rotateMatrix);
    }
}

void rotateX(struct point vertices[8], float angle)
{
    float rotateMatrix[] = {
        1,      0,              0,        
        0,      cos(angle),     -sin(angle),
        0,      sin(angle),     cos(angle)
    };
    rotate(vertices, rotateMatrix);
}
void rotateY(struct point vertices[8], float angle)
{
    float rotateMatrix[] = {
        cos(angle),     0,      sin(angle),    
        0,              1,      0,        
        -sin(angle),    0,      cos(angle)
    };
    rotate(vertices, rotateMatrix);
}
void rotateZ(struct point vertices[8], float angle)
{
    float rotateMatrix[] = {
        cos(angle),     -sin(angle),    0,
        sin(angle),     cos(angle),     0,
        0,              0,              1        
    };
    rotate(vertices, rotateMatrix);
}

struct point polToAf(struct polPoint pol)
{
    struct point af = {
        pol.r*sin(pol.zen)*cos(pol.az),
        pol.r*sin(pol.zen)*sin(pol.az),
        pol.r*cos(pol.zen)
    };
    // struct point af = {
    //     pol.r,
    //     pol.zen,
    //     pol.az
    // };
    return af;
}

struct polPoint afToPol(struct point af)
{
    struct polPoint pol = {
        sqrt(af.x*af.x + af.y*af.y + af.z*af.z),
        atan2(sqrt(af.x*af.x + af.y*af.y), af.z), //*2 - костыль
        atan2(af.y, af.x)
    };
    return pol;
}

void rotateZ1(struct point vertices[8], float angle)
{
    for (int i = 0; i < 8; i++){
        struct polPoint pol = afToPol(vertices[i]);
        pol.az += angle;
        vertices[i] = polToAf(pol);
    }
}

#include <stdio.h>

void DrawCube(ESContext *esContext, float size, int rotationZ)
{
    struct point vertices[] =  {
        {-0.5f,  -0.5f, 0.5f}, 
        {0.5f,  -0.5f, 0.5f}, 
        {0.5f,  0.5f, 0.5f}, 
        {-0.5f,  0.5f, 0.5f},
        {-0.5f,  -0.5f, -0.5f}, 
        {0.5f,  -0.5f, -0.5f}, 
        {0.5f,  0.5f, -0.5f}, 
        {-0.5f,  0.5f, -0.5f},
    };
    GLfloat vVertices[] = {
         -0.5f,  -0.5f, 0.0f, 
          0.5f,  -0.5f, 0.0f, 
           0.5f,  0.5f, 0.0f, 
            -0.5f,  0.5f, 0.0f
    };
    // rotateX(vertices, 0.21);
    // rotateY(vertices, 0.21);
    rotateZ1(vertices, 1);
    for (int i = 0; i < 4; i++){
        printf("%f,\t%f,\t%f,\t", vertices[i].x, vertices[i].y, vertices[i].z);
        printf("\n");
    }
    struct point points[] = {vertices[0],vertices[1],vertices[2],vertices[3]};
    DrawQuadranglePoints(esContext, points);
} 

void Draw ( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    // No clientside arrays, so do this in a webgl-friendly manner
    GLuint vertexPosObject;
    glGenBuffers(1, &vertexPosObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
    // Set the viewport
    glViewport ( 0, 0, esContext->width, esContext->height );
    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );
    // Use the program object
    glUseProgram ( userData->programObject );
    // Load the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vertexPosObject);
    glEnableVertexAttribArray(0);
    DrawCube(esContext, 0.5, 0);
}

int main ( int argc, char *argv[] )
{
    ESContext esContext;
    UserData  userData;

    esInitContext ( &esContext );
    esContext.userData = &userData;

    esCreateWindow ( &esContext, "Hello Triangle", 720, 720, ES_WINDOW_RGB );

    if ( !Init ( &esContext ) )
        return 0;

    esRegisterDrawFunc ( &esContext, Draw );

    esMainLoop ( &esContext );
}
