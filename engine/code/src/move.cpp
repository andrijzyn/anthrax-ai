#include "anthraxAI/vkengine.h"

void Engine::updatebones(int id, int objind) {
    if (Debug.animprepared) {
   	int frameind = FrameIndex % MAX_FRAMES_IN_FLIGHT;

        float timesec = (float)((double)getcurtime() - (double)Debug.startms) / 1000.0f;
		std::vector<glm::mat4> vec = animator.getbonestransform(Builder.getmodel(103), 103, timesec * Debug.animspeed);

        for(int i = 0; (i < vec.size() ); i++) {
        
            animtransf.bonesmatrices[i] = vec[i] ;

            
        }
 char* datadst;
   	const size_t sceneParamBufferSize = MAX_FRAMES_IN_FLIGHT * Builder.descriptors.paduniformbuffersize(sizeof(AnimationTransforms));
  	vkMapMemory(Builder.getdevice(), Builder.descriptors.gettransformbuffer()[FrameIndex].devicememory, 0, sceneParamBufferSize, 0, (void**)&datadst);
	datadst += (0 * Builder.descriptors.paduniformbuffersize(sizeof(AnimationTransforms)) * frameind);
    memcpy( datadst, &animtransf, (size_t)sizeof(AnimationTransforms));
  	vkUnmapMemory(Builder.getdevice(), Builder.descriptors.gettransformbuffer()[FrameIndex].devicememory);

		
        
        std::vector<glm::mat4> vec2 = animator.getbonestransform(Builder.getmodel(104), 104, timesec * Debug.animspeed);
memset(animtransf.bonesmatrices, 0, sizeof(animtransf.bonesmatrices));
        for(int i = 0; (i < vec2.size() ); i++) {
//             if (id == 104) {
//             animtransf.bonesmatrices[i] = glm::mat4(1.0f);
// // printf("\t hahahahaahaahah\n\n");
//             }
//             else {
            animtransf.bonesmatrices[i] = vec2[i];

            //}
        }
    
    char* datadst2;
 const size_t  sceneParamBufferSize2 = MAX_FRAMES_IN_FLIGHT * Builder.descriptors.paduniformbuffersize(sizeof(AnimationTransforms));
  	vkMapMemory(Builder.getdevice(), Builder.descriptors.gettransformbuffer()[FrameIndex].devicememory, 0, sceneParamBufferSize2, 0, (void**)&datadst2);
	datadst2 += ( 1 * Builder.descriptors.paduniformbuffersize(sizeof(AnimationTransforms)) * frameind);
    memcpy( datadst2, &animtransf, (size_t)sizeof(AnimationTransforms));
  	vkUnmapMemory(Builder.getdevice(), Builder.descriptors.gettransformbuffer()[FrameIndex].devicememory);
        // char* storagetmp;
        // vkMapMemory(Builder.getdevice(), Builder.descriptors.getstoragebuffer()[FrameIndex].devicememory, 0, sizeof(glm::mat4 )* MAX_BONES, 0, (void**)&storagetmp);
        // memcpy( storagetmp, &storagedata, (size_t)sizeof(glm::mat4)  * MAX_BONES);
        // vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getstoragebuffer()[FrameIndex].devicememory);
    }
memset(animtransf.bonesmatrices, 0, sizeof(animtransf.bonesmatrices));

}

void Engine::preparecamerabuffer() {
    glm::mat4 view = glm::lookAt(EditorCamera.getposition(), EditorCamera.getposition() + EditorCamera.getfront(), EditorCamera.getup());
	glm::mat4 projection = glm::perspective(glm::radians(45.f), static_cast<float>(Builder.getswapchainextent().width / Builder.getswapchainextent().height), 0.01f, 100.0f);
	projection[1][1] *= -1;
	glm::mat4 modell = glm::mat4(1.0f);
	modell = glm::translate(modell, glm::vec3(camdata.lightpos.x,camdata.lightpos.y,camdata.lightpos.z));
	modell = glm::scale(modell, glm::vec3(0.2f));

	camdata.model = modell;
	camdata.proj = projection;
	camdata.view = view;
	camdata.viewproj = projection * view;
	camdata.viewpos = glm::vec4(0, 0, 0, 1.0);
	camdata.mousepos = {Mouse.pos.x, Mouse.pos.y, 0, 0};
	camdata.viewport = {WindowExtend.width, WindowExtend.height, 0, 0};

	char* datadst;
   	const size_t sceneParamBufferSize = MAX_FRAMES_IN_FLIGHT * Builder.descriptors.paduniformbuffersize(sizeof(CameraData));
  	vkMapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameIndex].devicememory, 0, sceneParamBufferSize, 0, (void**)&datadst);
   	int frameind = FrameIndex % MAX_FRAMES_IN_FLIGHT;
	datadst += Builder.descriptors.paduniformbuffersize(sizeof(CameraData)) * frameind;
    memcpy( datadst, &camdata, (size_t)sizeof(CameraData));
  	vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getcamerabuffer()[FrameIndex].devicememory);

}


void Engine::mousepicking() {

    if (Mouse.state != MOUSE_PRESSED) {
        return;
    }
    
    if ( state & PLAY_GAME) {
        std::vector<RenderObject>& rq = Builder.getrenderqueue();
        std::vector<RenderObject>::iterator model_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.selected == true && val.type == TYPE_MODEL; } );;
        if (model_it != rq.end()) {
            model_it->selected = false;;
            gizmomove.visible = false;
        }
        return;
    }

    void* storagetmp;
    VkDeviceSize storagesize = sizeof(uint )* DEPTH_ARRAY_SCALE;
    vkMapMemory(Builder.getdevice(), Builder.descriptors.getstoragebuffer()[FrameIndex].devicememory, 0, storagesize, 0, (void**)&storagetmp);
    uint* u = static_cast<uint*>(storagetmp);

    int selectedID = -1;
    for (int i = 0; i < DEPTH_ARRAY_SCALE; i++) {
        if (u[i] != 0) {
            selectedID = u[i];
            Mouse.state = MOUSE_SELECTED;
            break;
        }

    }
    std::memset(storagetmp, 0, DEPTH_ARRAY_SCALE * sizeof(uint32_t));
    vkUnmapMemory(Builder.getdevice(), Builder.descriptors.getstoragebuffer()[FrameIndex].devicememory);

    bool selected = false;
    RenderObject* rq = Builder.getrenderqueue().data();
    for (int i = 0; i < Builder.getrenderqueue().size(); i++)
    {
        RenderObject& object = rq[i];

        if (object.type == TYPE_MODEL || object.type == TYPE_GIZMO) {
            if (object.ID == selectedID) {
                object.selected = true;
                selected = true;
                if (object.type != TYPE_GIZMO) {
                    gizmomove.objecthandler = selectedID;
                    gizmomove.visible = true;
                }
                else {
                    gizmomove.axis = static_cast<GizmoAxis>(object.ID);
                }
            }
            else {
                object.selected = false;
            }
        }
    }
    if (!selected) {
        gizmomove.visible = false;
    }
}

void Engine::update3d() {
	   
    std::vector<RenderObject>& rq = Builder.getrenderqueue();
    
    std::vector<RenderObject>::iterator gizmo_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.type == TYPE_GIZMO; } );
    if (gizmo_it == rq.end()) {
        return;
    }
    std::vector<RenderObject>::iterator model_it = std::find_if(rq.begin(), rq.end(), [&](const RenderObject& val){ return val.ID == gizmomove.objecthandler && val.type == TYPE_MODEL; } );;
    if (model_it == rq.end()) {
        return;
    }

    if ((Mouse.posdelta.x < -1 || Mouse.posdelta.x > 1) && gizmomove.axis == AXIS_X) {
        model_it->pos.x -= 0.06 *  Mouse.posdelta.x ;
    }
    if ((Mouse.posdelta.y < -1 || Mouse.posdelta.y > 1) && gizmomove.axis == AXIS_Y) {
        model_it->pos.y += 0.06 *  Mouse.posdelta.y ;
    }
    if ((Mouse.posdelta.y < -1 || Mouse.posdelta.y > 1) && gizmomove.axis == AXIS_Z) {
        model_it->pos.z += 0.06 *  Mouse.posdelta.y;
    }
    model_it->selected = true;
    
    gizmo_it->pos = model_it->pos;
    gizmo_it->pos.y += GIZMO_HEIGHT;
    ++gizmo_it;
    gizmo_it->pos = model_it->pos;
    gizmo_it->pos.y += GIZMO_HEIGHT;
    ++gizmo_it;
    gizmo_it->pos = model_it->pos;
    gizmo_it->pos.y += GIZMO_HEIGHT;

}

void Engine::update() {
    if (Level.test3d) return;

    for (int i = 0; i < Level.getobject().size(); i++) {
        if (!Level.getobject()[i]->update) {
            continue;
        }
        Positions sizes;

        if (Level.getobject()[i]->animation) {
            sizes = Positions(int(Level.getobject()[i]->getanimator()[0].getanimsize().x), int(Level.getobject()[i]->getanimator()[0].getanimsize().y) );
        }
        else {
            sizes = Positions(int(Builder.gettexture(Level.getobject()[i]->getpath())->w), int(Builder.gettexture(Level.getobject()[i]->getpath())->h) );
        }
        Builder.updatemesh(Builder.getmesh(TYPE_OBJECT + Level.getobject()[i]->ID), sizes, Level.getobject()[i]->getposition());
    }

    if (Level.getplayer()->update) {
        Positions sizes;
        if (Level.getplayer()->animation) {
            sizes = Positions(int(Level.getplayer()->getanimator()[0].getanimsize().x), int(Level.getplayer()->getanimator()[0].getanimsize().y) );
        }
        else {
            sizes = Positions(int(Builder.gettexture(Level.getplayer()->getpath())->w), int(Builder.gettexture(Level.getplayer()->getpath())->h) );
            Level.getplayer()->update = false;
        }
        Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), sizes, Level.getplayer()->getposition());
    }
}

void Engine::move() {
    if (Level.test3d) return;
    processanimation();
    processtrigger();
    moveplayer();
}

bool Engine::collision(int& state, bool collision, Positions pos, Positions sizes) {
    if (!collision || Level.getobject().empty()) {
        return true;
    }

    for (int i = 0; i < Level.getobject().size(); i++) {
        Positions objsizes;
        if (Level.getobject()[i]->animation) {
            objsizes = Positions(int(Level.getobject()[i]->getanimator()[0].getanimsize().x), int(Level.getobject()[i]->getanimator()[0].getanimsize().y) );
        }
        else {
            objsizes = Positions(int(Builder.gettexture(Level.getobject()[i]->getpath())->w), int(Builder.gettexture(Level.getobject()[i]->getpath())->h) );
        }
     
        if (pos.x < Level.getobject()[i]->getposition().x + objsizes.x &&
        pos.x + sizes.x > Level.getobject()[i]->getposition().x &&
        pos.y < Level.getobject()[i]->getposition().y + objsizes.y &&
        pos.y + sizes.y > Level.getobject()[i]->getposition().y ) {
            return true;
        }
    }
    state |= MOVE_DOWN;
    return false;
}

void Engine::moveplayer() {
    if (Level.getplayer()->getpath() != "") {
        Positions tmp = Level.getplayer()->getposition();
        Positions tmp2 = tmp;

        Positions sizes;
        if (Level.getplayer()->animation) {
           sizes = Positions(int(Level.getplayer()->getanimator()[0].getanimsize().x), int(Level.getplayer()->getanimator()[0].getanimsize().y) );
        }
        else {
            sizes = Positions(int(Builder.gettexture(Level.getplayer()->getpath())->w), int(Builder.gettexture(Level.getplayer()->getpath())->h) );
        }
    
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y + 8}, sizes) && Level.getplayer()->state & MOVE_DOWN) {
            tmp.y += 8;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x, Level.getplayer()->getposition().y - 32}, sizes) && Level.getplayer()->state & MOVE_UP) {
            tmp.y -= 16;
            Level.getplayer()->state ^= MOVE_UP;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x - 5, Level.getplayer()->getposition().y}, sizes) && Level.getplayer()->state & MOVE_LEFT) {
            tmp.x -= 10;
        }
        if (!collision(Level.getplayer()->state, Level.getplayer()->collision, {Level.getplayer()->getposition().x + 5, Level.getplayer()->getposition().y}, sizes) && Level.getplayer()->state & MOVE_RIGHT) {
            tmp.x += 10;
        }
        if (tmp.x != tmp2.x || tmp.y != tmp2.y) {
            Level.getplayer()->setposition(tmp);
            Level.getplayer()->update = true;
            //Builder.updatemesh(Builder.getmesh(TYPE_PLAYER), TYPE_PLAYER, Level.getplayer()->getposition());
        }
	}
}
