/*
 * Copyright (C) 2015 The Android Open Source Project
 */
#include "context.h"
#include "slot.h"

/*******************************************************************************
 * createShader
 * Create a shader object, load the shader source, and  compile the shader
 * @param shaderType
 * @param pSource
 * @return 
 *******************************************************************************/

GLuint createShader(GLenum shdType, const char* shdSource) {
    GLuint shader;
    GLint compiled;
    // Create the shader object
    shader = glCreateShader(shdType);
    if (shader) {
        // Load the shader source
        glShaderSource(shader, 1, &shdSource, NULL);
        // Compile the shader
        glCompileShader(shader);
        // Check the compile status
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = (char*) malloc(infoLen);
                glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
                //outDebugMessge("createShader:: Compile error %s \n", infoLog);
                free(infoLog);
                glDeleteShader(shader);
                shader = 0;
            }
            //outDebugMessge("createShader::  Could not compile the shader \n");
        }
    } else {
        shader = 0;
        //outDebugMessge("createShader::  Could not create the shader \n");

    }
    return shader;
}

/*******************************************************************************
 * createProgram
 * @param pVertexSource
 * @param pFragmentSource
 * @return 
 *******************************************************************************/


GLuint createProgram(const char* vertexShdSource, const char* fragmentShdSource) {
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint program;
    // Load the vertex/fragment shaders        return 0;

    vertexShader = createShader(GL_VERTEX_SHADER, vertexShdSource);
    if (!vertexShader) {
        return 0;
    }
    //outDebugMessge("createProgram::  Vertex shader is OK!");

    fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShdSource);
    if (!fragmentShader) {
    }
    //outDebugMessge("createProgram::  Fragment shader is OK!");
    // Create the program object
    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        // Link the program
        glLinkProgram(program);
        GLint linked;
        // Check the link status
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            GLint infoLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char* infoLog = (char *) malloc(infoLen);
                glGetProgramInfoLog(program, infoLen, NULL, infoLog);
                //outDebugMessage("createProgram:: Link error %s\n", infoLog);
                free(infoLog);
                glDeleteProgram(program);
                program = 0;
            }
            //outDebugMessage("createProgram::  Could not link the  program object");
        }
        //outDebugMessage("createProgram::  Program  is OK!");
    } else {
        program = 0;
        outDebugMessage("createProgram::  Could not create the  program object");

    }
    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}


/*******************************************************************************
 * deleteGraphicObjects
 * @param engine
 * @return 
 *******************************************************************************/
bool deleteGraphicObjects(Engine * engine) {
    glDeleteBuffers(1, &engine->VBO_vertexBuffer);
    glDeleteBuffers(1, &engine->VBO_indexBuffer);
    for (int i = 0; i < 7; i++) {
        glDeleteTextures(1, &engine->texture[i]);
        engine->texture[i] = 0;
    }
}

/*******************************************************************************
 * initGraphicObjects
 * @param engine
 * @return 
 *******************************************************************************/
bool initGraphicObjects(Engine * engine) {

    ShaderData *shaderData = &engine->shaderData;
    // Set program handles for shape drawing. These will later be used to pass in values to the shader program.
    shaderData->shapeProgram = createProgram(vShaderShape, fShaderShape);

    //    shaderData->shapeMatrixPHandle = glGetUniformLocation(shaderData->shapeProgram, "u_PMatrix");
    shaderData->shapeMatrixMVHandle = glGetUniformLocation(shaderData->shapeProgram, "u_MVMatrix");
    shaderData->shapeMatrixMVPHandle = glGetUniformLocation(shaderData->shapeProgram, "u_MVPMatrix");
    shaderData->shapePosHandle = glGetAttribLocation(shaderData->shapeProgram, "a_Position");
    shaderData->shapeColorHandle = glGetAttribLocation(shaderData->shapeProgram, "a_Color");
    shaderData->shapeTexCoordHandle = glGetAttribLocation(shaderData->shapeProgram, "TexCoordIn");
    shaderData->shapeTextureHandle = glGetUniformLocation(shaderData->shapeProgram, "Texture");
    shaderData->shapeNormalHandle = glGetAttribLocation(shaderData->shapeProgram, "a_Normal");
    shaderData->shapeLightPosHandle = glGetUniformLocation(shaderData->shapeProgram, "u_LightPos");
    shaderData->shapeULightHandle = glGetUniformLocation(shaderData->shapeProgram, "u_Light");


    // Set program handles for light drawing. These will later be used to pass in values to the shader program.
    shaderData->lightProgram = createProgram(vShaderPoint, fShaderPoint);

    shaderData->lightMatrixMVPHandle = glGetUniformLocation(shaderData->lightProgram, "u_MVPMatrix");
    shaderData->lightPosHandle = glGetAttribLocation(shaderData->lightProgram, "a_Position");
    shaderData->lightUColorHandle = glGetUniformLocation(shaderData->lightProgram, "u_Color");
    //    shaderData->lightColorHandle = glGetAttribLocation(shaderData->lightProgram, "a_Color");
    shaderData->lightNormalHandle = glGetAttribLocation(shaderData->lightProgram, "a_Normal");
    shaderData->lightULightHandle = glGetUniformLocation(shaderData->lightProgram, "u_Light");


    // Set program handles for ground drawing. These will later be used to pass in values to the shader program.
    shaderData->groundProgram = createProgram(vShaderGround, fShaderGround);

    shaderData->groundMatrixMVPHandle = glGetUniformLocation(shaderData->groundProgram, "u_MVPMatrix");
    shaderData->groundPosHandle = glGetAttribLocation(shaderData->groundProgram, "a_Position");
    //shaderData->groundColorHandle = glGetAttribLocation(shaderData->groundProgram, "a_Color");
    shaderData->groundNormalHandle = glGetAttribLocation(shaderData->groundProgram, "a_Normal");
    shaderData->groundLightPosHandle = glGetUniformLocation(shaderData->groundProgram, "u_LightPos");
    shaderData->groundLight2PosHandle = glGetUniformLocation(shaderData->groundProgram, "u_Light2Pos");
    shaderData->groundULightHandle = glGetUniformLocation(shaderData->groundProgram, "u_LightF");
    shaderData->groundUDifFHandle = glGetUniformLocation(shaderData->groundProgram, "u_DifF");
    shaderData->groundUTexFHandle = glGetUniformLocation(shaderData->groundProgram, "u_TexF");
    shaderData->groundTexCoordHandle = glGetAttribLocation(shaderData->groundProgram, "TexCoordIn");
    shaderData->groundTextureHandle = glGetUniformLocation(shaderData->groundProgram, "Texture");


    // Set program handles for scene drawing. These will later be used to pass in values to the shader program.
    shaderData->sceneProgram = createProgram(vShaderScene, fShaderScene);

    shaderData->sceneMatrixMVPHandle = glGetUniformLocation(shaderData->sceneProgram, "u_MVPMatrix");
    shaderData->scenePosHandle = glGetAttribLocation(shaderData->sceneProgram, "a_Position");
    shaderData->sceneColorHandle = glGetAttribLocation(shaderData->sceneProgram, "a_Color");
    shaderData->sceneTexCoordHandle = glGetAttribLocation(shaderData->sceneProgram, "TexCoordIn");
    shaderData->sceneTextureHandle = glGetUniformLocation(shaderData->sceneProgram, "Texture");

    // Set program handles for square drawing. These will later be used to pass in values to the shader program.
    shaderData->squareProgram = createProgram(vShaderSquare, fShaderSquare);

    shaderData->squareMatrixMVPHandle = glGetUniformLocation(shaderData->squareProgram, "u_MVPMatrix");
    shaderData->squarePosHandle = glGetAttribLocation(shaderData->squareProgram, "a_Position");
    shaderData->squareUColorHandle = glGetUniformLocation(shaderData->squareProgram, "u_Color");
    shaderData->squareTexCoordHandle = glGetAttribLocation(shaderData->squareProgram, "TexCoordIn");
    shaderData->squareTextureHandle = glGetUniformLocation(shaderData->squareProgram, "Texture");



    // Set program handles for font drawing. These will later be used to pass in values to the shader program.
    shaderData->fontProgram = createProgram(vShaderFont, fShaderFont);

    shaderData->fontUColorHandle = glGetUniformLocation(shaderData->fontProgram, "u_Color");
    shaderData->fontTexCoordHandle = glGetAttribLocation(shaderData->fontProgram, "TexCoordIn");
    shaderData->fontTextureHandle = glGetUniformLocation(shaderData->fontProgram, "Texture");

    // Set program handles for font drawing. These will later be used to pass in values to the shader program.
    shaderData->randomProgram = createProgram(vShaderRandom, fShaderRandom);

    shaderData->randomUColorHandle = glGetUniformLocation(shaderData->randomProgram, "u_Color");
    shaderData->randomUPosHandle = glGetUniformLocation(shaderData->randomProgram, "u_Position");
    shaderData->randomMatrixMVPHandle = glGetUniformLocation(shaderData->randomProgram, "u_MVPMatrix");
    shaderData->randomPosHandle = glGetAttribLocation(shaderData->randomProgram, "a_Position");

    std::string lst;
    for (int i = 0; i < 7; i++) {
        lst = *engine->state.shape[i].lst_iterator;
        loadTextureFromPNG(engine, lst.c_str(), engine->twidth, engine->theight, i);
    }
    initFTresources(engine);
    getDate(engine);
    initFontsColor(engine);
    return true;
}




/*******************************************************************************
 * draw Sphere.
 ******************************************************************************/

void drawSphere(Engine * engine, int j) {
    if (!engine->state.shape[j].isVisible) return;
    if (engine->state.shape[j].isBlendOn) {
        //  glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_SRC_COLOR);
    }
    float radius;
    int p = 50;
    ShaderData *shaderData = &engine->shaderData;
    M4 *m4 = &engine->m4;
    V4 *v4 = &engine->v4;
    m4->GroundModelMatrix = glm::mat4(1.0f);
    rotateShape(engine, j);
    allocMemorySpheresVBO(engine, p);
    radius = motionSphere(engine, j);
    renderSphere(engine, radius, p);
    createVBO(engine);
    changeShapesTexture(engine, j);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, engine->texture[j]);
    glUseProgram(shaderData->groundProgram);
    glUniform1i(shaderData->groundTextureHandle, 0);
    m4->GroundModelMatrix = glm::translate(m4->GroundModelMatrix, engine->state.shape[j].startM);
    if (engine->state.isEditOn) {
        if (engine->ratio < 1)
            m4->GroundModelMatrix = glm::translate(m4->GroundModelMatrix, glm::vec3(0.0, 0.0, 0.0)); // -3.50f + 
    }
    m4->GroundModelMatrix = glm::rotate(m4->GroundModelMatrix, degreesToRadians(engine->state.shape[j].angleR.y), glm::vec3(0.0f, 1.0f, 0.0f));
    m4->MVMatrix = m4->ViewMatrix * m4->GroundModelMatrix;
    m4->MVPMatrix = m4->ProjectionMatrix * m4->MVMatrix;
    glUniformMatrix4fv(shaderData->groundMatrixMVPHandle, 1, 0, glm::value_ptr(m4->MVPMatrix));
    glUniform3f(shaderData->groundLightPosHandle, v4->LightPosInEyeSpace.x, v4->LightPosInEyeSpace.y, v4->LightPosInEyeSpace.z);
    glUniform3f(shaderData->groundLight2PosHandle, v4->Light2PosInEyeSpace.x, v4->Light2PosInEyeSpace.y, v4->Light2PosInEyeSpace.z);
    glUniform1i(shaderData->groundULightHandle, engine->state.shape[j].uLightF);
    glUniform1i(shaderData->groundUDifFHandle, engine->state.shape[j].uDiffuseF);
    glUniform1i(shaderData->groundUTexFHandle, engine->state.shape[j].uTextureF);

    glEnableVertexAttribArray(shaderData->groundNormalHandle);
    glEnableVertexAttribArray(shaderData->groundPosHandle);
    glEnableVertexAttribArray(shaderData->groundTexCoordHandle);
    glVertexAttribPointer(shaderData->groundPosHandle, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (BUFFER_OFFSET(engine->offsetV)));
    glVertexAttribPointer(shaderData->groundNormalHandle, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (BUFFER_OFFSET(engine->offsetN)));
    glVertexAttribPointer(shaderData->groundTexCoordHandle, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*) (BUFFER_OFFSET(engine->offsetT)));
    if (engine->state.shape[j].isGridOn)
        glDrawElements(GL_LINES, engine->sizeI / sizeof (GLushort), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    else
        glDrawElements(GL_TRIANGLES, engine->sizeI / sizeof (GLushort), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
    glDisableVertexAttribArray(shaderData->groundPosHandle);
    glDisableVertexAttribArray(shaderData->groundNormalHandle);
    glDisableVertexAttribArray(shaderData->groundTexCoordHandle);

    freeMemorySpheresVBO(engine);
    if (engine->state.shape[j].isBlendOn) {
        glDisable(GL_BLEND);

    }

}

/*******************************************************************************
 * createVBO
 * @param engine
 *******************************************************************************/
void createVBO(Engine * engine) {
    glDeleteBuffers(1, &engine->VBO_vertexBuffer);
    glDeleteBuffers(1, &engine->VBO_indexBuffer);
    int sizeBuff = engine->sizeV + engine->sizeC + engine->sizeN + engine->sizeT;
    engine->offsetV = 0;
    engine->offsetC = engine->offsetV + engine->sizeV;
    engine->offsetN = engine->offsetC + engine->sizeC;
    engine->offsetT = engine->offsetN + engine->sizeN;


    glGenBuffers(1, &engine->VBO_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, engine->VBO_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeBuff, NULL, GL_STATIC_DRAW);
    if (engine->sizeV != 0) glBufferSubData(GL_ARRAY_BUFFER, engine->offsetV, engine->sizeV, &engine->sphereVertices[0]);
    if (engine->sizeC != 0) glBufferSubData(GL_ARRAY_BUFFER, engine->offsetC, engine->sizeC, &engine->sphereColors[0]);
    if (engine->sizeN != 0) glBufferSubData(GL_ARRAY_BUFFER, engine->offsetN, engine->sizeN, &engine->sphereNormals[0]);
    if (engine->sizeT != 0) glBufferSubData(GL_ARRAY_BUFFER, engine->offsetT, engine->sizeT, &engine->sphereTextures[0]);
    glBindBuffer(GL_ARRAY_BUFFER, engine->VBO_vertexBuffer);
    glGenBuffers(1, &engine->VBO_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, engine->VBO_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, engine->sizeI, &engine->sphereIndices[0], GL_STATIC_DRAW);





}

/*******************************************************************************
 * initMemoryForDrawingSphere
 * @param engine
 * @param p
 *******************************************************************************/
void allocMemorySpheresVBO(Engine *engine, int p) {
    engine->sizeV = ((p + 1) * (p + 1) * 3) * sizeof (GLfloat);
    engine->sizeN = ((p + 1) * (p + 1) * 3) * sizeof (GLfloat);
    engine->sizeT = ((p + 1) * (p + 1) * 2) * sizeof (GLfloat);
    engine->sizeI = ((p + 1) * (p + 1) * 6) * sizeof (GLushort);
    if (engine->sphereVertices == NULL)
        engine->sphereVertices = (GLfloat*) malloc(engine->sizeV);
    if (engine->sphereNormals == NULL)
        engine->sphereNormals = (GLfloat*) malloc(engine->sizeN);
    if (engine->sphereTextures == NULL)
        engine->sphereTextures = (GLfloat*) malloc(engine->sizeT);
    if (engine->sphereIndices == NULL)
        engine->sphereIndices = (GLushort*) malloc(engine->sizeI);
    engine->sizeC = 0;
    engine->sphereColors = NULL;
}

/*******************************************************************************
 * FreeMemoryForDrawingSphere
 * @param engine
 *******************************************************************************/

void freeMemorySpheresVBO(Engine * engine) {
    if (engine->sphereVertices != NULL) free(engine->sphereVertices);
    if (engine->sphereNormals != NULL) free(engine->sphereNormals);
    if (engine->sphereTextures != NULL) free(engine->sphereTextures);
    if (engine->sphereIndices != NULL) free(engine->sphereIndices);
    resetPointerVBO(engine);
}


/*******************************************************************************
 * changeShapesTexture
 * @param engine
 *******************************************************************************/
void changeShapesTexture(Engine* engine, int j) {
    if (engine->state.shape[j].isTextureChanged) {
        std::string lst;
        if (engine->state.shape[j].lst_iterator == engine->listAssets.end())
            engine->state.shape[j].lst_iterator = engine->listAssets.begin();
        lst = *engine->state.shape[j].lst_iterator;
        loadTextureFromPNG(engine, lst.c_str(), engine->twidth, engine->theight, j);
        engine->state.shape[j].isTextureChanged = false;


    }
}



/*******************************************************************************
 * 
 * touchPane
 * 
 ******************************************************************************/
int touchPane(Engine* engine) {
    for (int i = 0; i < 11; i++) {
        if ((engine->state.x < engine->pane[i].x1) && (engine->state.x > engine->pane[i].x)) {
            if ((engine->state.y < engine->pane[i].y1)&& (engine->state.y > engine->pane[i].y)) return i + 1;
        }
    }
    return 0;
}

