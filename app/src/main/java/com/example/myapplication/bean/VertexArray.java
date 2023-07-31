/***
 * Excerpted from "OpenGL ES for Android",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material, 
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose. 
 * Visit http://www.pragmaticprogrammer.com/titles/kbogla for more book information.
***/
package com.example.myapplication.bean;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import static android.opengl.GLES20.GL_FLOAT;
import static android.opengl.GLES20.glEnableVertexAttribArray;
import static android.opengl.GLES20.glVertexAttribPointer;
import static com.example.myapplication.bean.Constants.BYTES_PER_FLOAT;


public class VertexArray {    
    public final FloatBuffer floatBuffer;

    public VertexArray(float[] vertexData) {
        floatBuffer = ByteBuffer
            .allocateDirect(vertexData.length * BYTES_PER_FLOAT)
            .order(ByteOrder.nativeOrder())
            .asFloatBuffer()
            .put(vertexData);
    }


    /**
     * vertexArray.setVertexAttribPointer(
     *                 0,
     *                 lutProgram.getPositionAttributeLocation(),
     *                 2,
     *                 16);
     *
     *         vertexArray.setVertexAttribPointer(
     *                 2,
     *                 lutProgram.getTextureCoordinatesAttributeLocation(),
     *                 2,
     *                 16);
     * @param dataOffset
     * @param attributeLocation
     * @param componentCount
     * @param stride
     */
    public void setVertexAttribPointer(int dataOffset, int attributeLocation,
        int componentCount, int stride) {        
        floatBuffer.position(dataOffset);        
        glVertexAttribPointer(attributeLocation, componentCount, GL_FLOAT, 
            false, stride, floatBuffer);
        glEnableVertexAttribArray(attributeLocation);
        
        floatBuffer.position(0);
    }
}
