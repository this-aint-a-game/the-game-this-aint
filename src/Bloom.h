//
// Created by Alexa Drenick on 2019-06-03.
//

#ifndef OBTAIN_BLOOM_H
#define OBTAIN_BLOOM_H

class Bloom
{
    GLuint frameBuf[2];
    GLuint texBuf[2];

    GLuint depthBuf;

    GLuint screenBuf[1];
    GLuint screenTexBuf[1];

    GLuint quad_VertexArrayID;
    GLuint quad_vertexbuffer;

    std::shared_ptr<Program> tex_prog;
    std::shared_ptr<Program> comb_prog;

    bool FirstTime = true;

public:

    void combine()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texBuf[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, screenTexBuf[0]);

        // example applying of 'drawing' the FBO texture
        comb_prog->bind();
        glUniform1i(comb_prog->getUniform("bloomBuf"), 0);
        glUniform1i(comb_prog->getUniform("sceneBuf"), 1);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);    //drawing a quad
        glDisableVertexAttribArray(0);
        comb_prog->unbind();
    }

    void init(int width, int height)
    {
        initQuad();

        //initTexProg();

        initCombProg();

        /*
        glGenFramebuffers(2, frameBuf);
        glGenTextures(2, texBuf);
        //glGenRenderbuffers(1, &depthBuf);
        createFBO(frameBuf[0], texBuf[0], width, height);

        // TODO depth buffer???
        //set up depth necessary since we are rendering a mesh that needs depth test
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);


        //more FBO set up
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);

        //create another FBO so we can swap back and forth
        createFBO(frameBuf[1], texBuf[1], width, height);
        //this one doesn't need depth
        */

        glGenFramebuffers(1, screenBuf);
        glGenTextures(1, screenTexBuf);
        glGenRenderbuffers(1, &depthBuf);
        createFBO(screenBuf[0], screenTexBuf[0], width, height);

        glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);
    }

    GLuint getScreenBuf()
    {
        return screenTexBuf[0];
    }

    GLuint getBlurBuffer()
    {
        return texBuf[1];
    }

    /* To call the blur on the specificed texture */
    void Blur(GLuint inTex, bool horizontal) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inTex);

        // example applying of 'drawing' the FBO texture
        tex_prog->bind();
        glUniform1i(tex_prog->getUniform("texBuf"), 0);
        glUniform1i(tex_prog->getUniform("horizontal"), horizontal);
        //glUniform1f(tex_prog->getUniform("time"), glfwGetTime());
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);    //drawing a quad
        glDisableVertexAttribArray(0);
        tex_prog->unbind();
    }

    void render(Butterfly & butterfly, ObjectCollection* oc, MatrixStack* view, MatrixStack* projection, glm::vec3 camera)
    {
        // frameBuf[0] is actually memory -> storing the data
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[0]);

        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);

        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind prog
        // render scene
        oc->drawScene(oc->objProg, view, projection, camera, butterfly.currentPos);
        butterfly.drawbutterfly(butterfly.butterflyProg, view, projection, camera, oc->gameplay);
        // unbind prog

        //regardless unbind the FBO
        // framebuffer 0 means the screen, default behavior
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* code to write out the FBO (texture) just once */
        if (FirstTime) {
            // textures used as input, framebuffer used as output
            assert(GLTextureWriter::WriteImage(texBuf[0],"Texture_output.png"));
            //FirstTime = 0;
        }

        bool horizontal = true;
        for (int i = 0; i <10; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[(i+1)%2]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Blur(texBuf[i%2], horizontal);
            horizontal = !horizontal;
        }

        /*
        for (int i = 0; i < 5; i++)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuf[(i+1)%2]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            Blur(texBuf[i%2], false);
        }
         */

        if (FirstTime) {
            // textures used as input, framebuffer used as output
            assert(GLTextureWriter::WriteImage(texBuf[0],"Texture_output_blur0.png"));
            //FirstTime = 0;
        }

        if (FirstTime) {
            // textures used as input, framebuffer used as output
            assert(GLTextureWriter::WriteImage(texBuf[0],"Texture_output_after.png"));
            //FirstTime = 0;
        }
        if (FirstTime) {
            // textures used as input, framebuffer used as output
            assert(GLTextureWriter::WriteImage(texBuf[1],"Texture_output_blur1.png"));
            FirstTime = 0;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void initTexProg()
    {
        //set up the shaders to blur the FBO decomposed just a placeholder pass thru now
        //TODO - modify and possibly add other shaders to complete blur
        tex_prog = make_shared<Program>();
        tex_prog->setVerbose(true);
        tex_prog->setShaderNames("../resources/blur_vert.glsl", "../resources/blur_frag.glsl");
        tex_prog->init();
        tex_prog->addUniform("texBuf");
        tex_prog->addUniform("horizontal");
        tex_prog->addAttribute("vertPos");
    }

    void initCombProg()
    {
        //set up the shaders to blur the FBO decomposed just a placeholder pass thru now
        //TODO - modify and possibly add other shaders to complete blur
        comb_prog = make_shared<Program>();
        comb_prog->setVerbose(true);
        comb_prog->setShaderNames("../resources/blur_vert.glsl", "../resources/comb_frag.glsl");
        comb_prog->init();
        comb_prog->addUniform("bloomBuf");
        comb_prog->addUniform("sceneBuf");
        comb_prog->addAttribute("vertPos");
    }

    void initQuad() {

        //now set up a simple quad for rendering FBO
        glGenVertexArrays(1, &quad_VertexArrayID);
        glBindVertexArray(quad_VertexArrayID);

        static const GLfloat g_quad_vertex_buffer_data[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                -1.0f,  1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f,  1.0f, 0.0f,
        };

        glGenBuffers(1, &quad_vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    }

    /*
    Helper function to create the framebuffer object and associated texture to write to
    */
    void createFBO(GLuint& fb, GLuint& tex, int width, int height) {
        //initialize FBO (global memory)

        //set up framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        //set up texture
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            cout << "Error setting up frame buffer - exiting" << endl;
            exit(0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};


#endif //OBTAIN_BLOOM_H
