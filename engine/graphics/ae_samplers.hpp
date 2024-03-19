/// \file ae_samplers.hpp
/// The AeSamplers class is defined.
#pragma once

// dependencies
#include "ae_engine_constants.hpp"
#include "ae_device.hpp"

// libraries

//std


namespace ae {

    /// An object to create and manage the different samplers available for the engine.
    class AeSamplers {
    public:
        /// Create the samplers for the specified device.
        /// \param t_device The device the samplers will be made to be compatible with.
        AeSamplers(AeDevice &t_device);

        /// Destroys the samplers.
        ~AeSamplers();

        /// Do not allow this class to be copied (2 lines below)
        AeSamplers(const AeSamplers&) = delete;
        AeSamplers& operator=(const AeSamplers&) = delete;

        /// Gets the default sampler which includes: Bilinear filtering, anisotropic filtering, tiling, and expects
        /// normalized texture coordinates.
        /// \return The default sampler.
        VkSampler& getDefaultSampler();

    private:

        /// Creates the default sampler.
        void createDefaultSampler();

        /// The device the samplers are created for.
        AeDevice &m_aeDevice;

        /// The default sampler.
        VkSampler m_defaultSampler = nullptr;

    protected:

    };

} // namespace ae