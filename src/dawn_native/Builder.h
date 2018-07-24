// Copyright 2017 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DAWNNATIVE_BUILDER_H_
#define DAWNNATIVE_BUILDER_H_

#include "dawn_native/Forward.h"
#include "dawn_native/RefCounted.h"

#include "dawn/dawncpp.h"

#include <string>

namespace backend {

    // This class implements behavior shared by all builders:
    //  - Tracking whether GetResult has been called already, needed by the autogenerated code to
    //    prevent operations on "consumed" builders.
    //  - The error status callback of the API. The callback is guaranteed to be called exactly once
    //    with an error, a success, or "unknown" if the builder is destroyed; also the builder
    //    callback cannot be called before either the object is destroyed or GetResult is called.
    //
    // It is possible for error to be generated before the error callback is registered when a
    // builder "set" function performance validation inline. Because of this we have to store the
    // status in the builder and defer calling the callback to GetResult.

    class BuilderBase : public RefCounted {
      public:
        // Used by the auto-generated validation to prevent usage of the builder
        // after GetResult or an error.
        bool CanBeUsed() const;
        DeviceBase* GetDevice();

        // Set the status of the builder to an error.
        void HandleError(const char* message);

        // Internal API, to be used by builder and BackendProcTable only.
        // Returns true for success cases, and calls the callback with appropriate status.
        bool HandleResult(RefCounted* result);

        // Dawn API
        void SetErrorCallback(dawn::BuilderErrorCallback callback,
                              dawn::CallbackUserdata userdata1,
                              dawn::CallbackUserdata userdata2);

      protected:
        BuilderBase(DeviceBase* device);
        ~BuilderBase();

        DeviceBase* const mDevice;
        bool mGotStatus = false;

      private:
        void SetStatus(dawn::BuilderErrorStatus status, const char* message);

        dawn::BuilderErrorCallback mCallback = nullptr;
        dawn::CallbackUserdata mUserdata1 = 0;
        dawn::CallbackUserdata mUserdata2 = 0;

        dawn::BuilderErrorStatus mStoredStatus = dawn::BuilderErrorStatus::Success;
        std::string mStoredMessage;

        bool mIsConsumed = false;
    };

    // This builder base class is used to capture the calls to GetResult and make sure that either:
    //  - There was an error, callback is called with an error and nullptr is returned.
    //  - There was no error, callback is called with success and a non-null T* is returned.
    template <typename T>
    class Builder : public BuilderBase {
      public:
        // Dawn API
        T* GetResult();

      protected:
        using BuilderBase::BuilderBase;

      private:
        virtual T* GetResultImpl() = 0;
    };

    template <typename T>
    T* Builder<T>::GetResult() {
        T* result = GetResultImpl();
        // An object can have been returned but failed its initialization, so if an error happened,
        // return nullptr instead of result.
        if (HandleResult(result)) {
            return result;
        } else {
            return nullptr;
        }
    }

}  // namespace backend

#endif  // DAWNNATIVE_BUILDER_H_